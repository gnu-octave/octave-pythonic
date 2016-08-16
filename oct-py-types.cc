/*

Copyright (C) 2016 Mike Miller

This file is part of Pytave.

Pytave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

Pytave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Pytave; see the file COPYING.  If not, see
<http://www.gnu.org/licenses/>.

*/

#if defined (HAVE_CONFIG_H)
#  include <config.h>
#endif

#include <octave/Cell.h>
#include <octave/oct-map.h>
#include <octave/quit.h>

#include "exceptions.h"
#include "oct-py-types.h"

// FIXME: only here to bootstrap nested conversions needed in this file
#include "octave_to_python.h"

namespace pytave
{

PyObject *
make_py_bool (bool value)
{
  if (value)
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}

PyObject *
make_py_complex (std::complex<double> value)
{
  Py_complex& py_complex_value = reinterpret_cast<Py_complex&> (value);
  return PyComplex_FromCComplex (py_complex_value);
}

PyObject *
make_py_float (double value)
{
  return PyFloat_FromDouble (value);
}

inline PyObject *
make_py_int (int32_t value)
{
#if PY_VERSION_HEX >= 0x03000000
  return PyLong_FromLong (value);
#else
  return PyInt_FromLong (value);
#endif
}

inline PyObject *
make_py_int (uint32_t value)
{
  return PyLong_FromUnsignedLong (value);
}

inline PyObject *
make_py_int (int64_t value)
{
#if (defined (HAVE_LONG_LONG) && (SIZEOF_LONG_LONG > SIZEOF_LONG))
  return PyLong_FromLongLong (value);
#else
  return PyLong_FromLong (value);
#endif
}

inline PyObject *
make_py_int (uint64_t value)
{
#if (defined (HAVE_LONG_LONG) && (SIZEOF_LONG_LONG > SIZEOF_LONG))
  return PyLong_FromUnsignedLongLong (value);
#else
  return PyLong_FromUnsignedLong (value);
#endif
}

PyObject *
make_py_numeric_value (const octave_value& value)
{
  if (value.is_scalar_type ())
    {
      if (value.is_bool_type ())
        return make_py_bool (value.bool_value ());

      else if (value.is_int8_type ())
        return make_py_int (value.int8_scalar_value ().value ());
      else if (value.is_int16_type ())
        return make_py_int (value.int16_scalar_value ().value ());
      else if (value.is_int32_type ())
        return make_py_int (value.int32_scalar_value ().value ());
      else if (value.is_int64_type ())
        return make_py_int (value.int64_scalar_value ().value ());

      else if (value.is_uint8_type ())
        return make_py_int (value.uint8_scalar_value ().value ());
      else if (value.is_uint16_type ())
        return make_py_int (value.uint16_scalar_value ().value ());
      else if (value.is_uint32_type ())
        return make_py_int (value.uint32_scalar_value ().value ());
      else if (value.is_uint64_type ())
        return make_py_int (value.uint64_scalar_value ().value ());

      else if (value.is_complex_type ())
        return make_py_complex (value.complex_value ());
      else if (value.is_float_type ())
        return make_py_float (value.double_value ());
    }

  throw value_convert_exception ("unhandled scalar type");
  return 0;
}

inline PyObject *
wrap_octvalue_to_pyobj (const octave_value& value)
{
  boost::python::object obj;
  octvalue_to_pyobj (obj, value);
  PyObject *ptr = obj.ptr ();
  Py_INCREF (ptr);
  return ptr;
}

PyObject *
make_py_dict (const octave_scalar_map& map)
{
  PyObject *dict = PyDict_New ();
  if (! dict)
    octave_throw_bad_alloc ();

  for (auto p = map.begin (); p != map.end (); ++p)
    {
      PyObject *key = make_py_str (map.key (p));
      if (! key)
        octave_throw_bad_alloc ();

      PyObject *item = wrap_octvalue_to_pyobj (map.contents (p));

      if (PyDict_SetItem (dict, key, item) < 0)
        throw boost::python::error_already_set ();
    }

  return dict;
}

int64_t
extract_py_int64 (PyObject *obj)
{
  if (! obj)
    throw object_convert_exception ("failed to extract integer: null object");

  if (PyLong_Check (obj))
    return PyLong_AsLong (obj);
#if PY_VERSION_HEX < 0x03000000
  else if (PyInt_Check (obj))
    return PyInt_AsLong (obj);
#endif
  else
    throw object_convert_exception ("failed to extract integer: wrong type");

  return 0;
}

PyObject *
make_py_list (const Cell& cell)
{
  if (! (cell.is_empty () || cell.is_vector ()))
    throw value_convert_exception (
      "unable to convert multidimensional cell array into Python sequence");

  PyObject *list = PyList_New (0);
  if (! list)
    octave_throw_bad_alloc ();

  for (octave_idx_type i = 0; i < cell.numel (); i++)
    {
      PyObject *item = wrap_octvalue_to_pyobj (cell.xelem (i));

      if (PyList_Append (list, item) < 0)
        throw boost::python::error_already_set ();
    }

  return list;
}

std::string
extract_py_str (PyObject *obj)
{
  std::string retval;

  if (! obj)
    throw object_convert_exception ("failed to extract string: null object");
  if (PyBytes_Check (obj))
    {
      retval.assign (PyBytes_AsString (obj), PyBytes_Size (obj));
    }
  else if (PyUnicode_Check (obj))
    {
      bool ok = false;
      PyObject *enc = PyUnicode_AsUTF8String (obj);
      if (enc)
        {
          if (PyBytes_Check (enc))
            {
              ok = true;
              retval.assign (PyBytes_AsString (enc), PyBytes_Size (enc));
            }
          Py_DECREF (enc);
        }
      if (! ok)
        throw object_convert_exception ("failed to extract string: UTF-8 error");
    }
  else
    throw object_convert_exception ("failed to extract string: wrong type");

  return retval;
}

PyObject *
make_py_str (const std::string& str)
{
#if PY_VERSION_HEX >= 0x03000000
  return PyUnicode_FromStringAndSize (str.data (), str.size ());
#else
  return PyString_FromStringAndSize (str.data (), str.size ());
#endif
}

}
