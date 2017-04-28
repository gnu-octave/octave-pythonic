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

#include <limits>
#include <octave/Cell.h>
#include <octave/oct-map.h>
#include <octave/quit.h>

#include "exceptions.h"
#include "oct-py-eval.h"
#include "oct-py-types.h"

// FIXME: only here to bootstrap nested conversions needed in this file
#include "octave_to_python.h"
#include "python_to_octave.h"

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

bool
extract_py_bool (PyObject *obj)
{
  if (! obj)
    throw object_convert_exception ("failed to extract boolean: null object");

  if (! PyBool_Check (obj))
    throw object_convert_exception ("failed to extract boolean: wrong type");

  return (obj == Py_True);
}

std::complex<double>
extract_py_complex (PyObject *obj)
{
  if (! obj)
    throw object_convert_exception ("failed to extract complex: null object");

  if (! PyComplex_Check (obj))
    throw object_convert_exception ("failed to extract complex: wrong type");

  Py_complex value = PyComplex_AsCComplex (obj);
  return reinterpret_cast<std::complex<double>&> (value);
}

double
extract_py_float (PyObject *obj)
{
  if (! obj)
    throw object_convert_exception ("failed to extract float: null object");

  if (! PyFloat_Check (obj))
    throw object_convert_exception ("failed to extract float: wrong type");

  return PyFloat_AsDouble (obj);
}

PyObject *
make_py_int (int32_t value)
{
#if PY_VERSION_HEX >= 0x03000000
  return PyLong_FromLong (value);
#else
  return PyInt_FromLong (value);
#endif
}

PyObject *
make_py_int (uint32_t value)
{
  return PyLong_FromUnsignedLong (value);
}

PyObject *
make_py_int (int64_t value)
{
#if (defined (HAVE_LONG_LONG) && (SIZEOF_LONG_LONG > SIZEOF_LONG))
  return PyLong_FromLongLong (value);
#else
  return PyLong_FromLong (value);
#endif
}

PyObject *
make_py_int (uint64_t value)
{
#if (defined (HAVE_LONG_LONG) && (SIZEOF_LONG_LONG > SIZEOF_LONG))
  return PyLong_FromUnsignedLongLong (value);
#else
  return PyLong_FromUnsignedLong (value);
#endif
}

PyObject *
make_py_array (const void *data, size_t len, char typecode)
{
  if (! typecode)
    throw object_convert_exception ("unable to create array from Octave data");

  std::string arg { typecode };
  PyObject *array = py_call_function ("array.array", ovl (arg));

  if (len > 0)
    {
      // create a byte buffer containing a copy of the array binary data
      const char *cdata = reinterpret_cast<const char *> (data);
      PyObject *buf = PyBytes_FromStringAndSize (cdata, len);
      if (! buf)
        octave_throw_bad_alloc ();

      PyObject *frombytes = (PyObject_HasAttrString (array, "frombytes") ?
                             PyObject_GetAttrString (array, "frombytes") :
                             PyObject_GetAttrString (array, "fromstring"));
      PyObject *args = PyTuple_Pack (1, buf);
      py_call_function (frombytes, args);
      Py_DECREF (args);
      Py_DECREF (buf);
    }

  return array;
}

// Prefer the 'q' and 'Q' typecodes if they are available (if Python 3 and
// built with support for long long integers)

#if (PY_VERSION_HEX >= 0x03000000) && defined (HAVE_LONG_LONG)
#  define ARRAY_INT64_TYPECODE 'q'
#  define ARRAY_UINT64_TYPECODE 'Q'
#elif (SIZEOF_LONG == 8)
#  define ARRAY_INT64_TYPECODE 'l'
#  define ARRAY_UINT64_TYPECODE 'L'
#else
#  define ARRAY_INT64_TYPECODE 0
#  define ARRAY_UINT64_TYPECODE 0
#endif

template <typename T>
struct py_array_info { };

template <>
struct py_array_info<octave_int8> { static const char typecode = 'b'; };

template <>
struct py_array_info<octave_int16> { static const char typecode = 'h'; };

template <>
struct py_array_info<octave_int32> { static const char typecode = 'i'; };

template <>
struct py_array_info<octave_int64>
{
  static const char typecode = ARRAY_INT64_TYPECODE;
};

template <>
struct py_array_info<octave_uint8> { static const char typecode = 'B'; };

template <>
struct py_array_info<octave_uint16> { static const char typecode = 'H'; };

template <>
struct py_array_info<octave_uint32> { static const char typecode = 'I'; };

template <>
struct py_array_info<octave_uint64> {
  static const char typecode = ARRAY_UINT64_TYPECODE;
};

PyObject *
make_py_array (const NDArray& nda)
{
  return make_py_array (nda.data (), nda.numel () * sizeof (double), 'd');
}

PyObject *
make_py_array (const FloatNDArray& nda)
{
  return make_py_array (nda.data (), nda.numel () * sizeof (float), 'f');
}

template <typename T>
PyObject *
make_py_array (const intNDArray<T>& nda)
{
  return make_py_array (nda.data (), nda.numel () * sizeof (T),
                        py_array_info<T>::typecode);
}

// Instantiate all possible integer array template functions needed

template PyObject * make_py_array<octave_int8> (const int8NDArray&);
template PyObject * make_py_array<octave_int16> (const int16NDArray&);
template PyObject * make_py_array<octave_int32> (const int32NDArray&);
template PyObject * make_py_array<octave_int64> (const int64NDArray&);
template PyObject * make_py_array<octave_uint8> (const uint8NDArray&);
template PyObject * make_py_array<octave_uint16> (const uint16NDArray&);
template PyObject * make_py_array<octave_uint32> (const uint32NDArray&);
template PyObject * make_py_array<octave_uint64> (const uint64NDArray&);

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

PyObject *
make_py_array (const octave_value& value)
{
  if (value.is_numeric_type () && ! value.is_complex_type ()
      && value.ndims () == 2 && (value.columns () <= 1 || value.rows () <= 1))
    {
      if (value.is_double_type ())
        return make_py_array (value.array_value ());
      else if (value.is_single_type ())
        return make_py_array (value.float_array_value ());

      else if (value.is_int8_type ())
        return make_py_array (value.int8_array_value ());
      else if (value.is_int16_type ())
        return make_py_array (value.int16_array_value ());
      else if (value.is_int32_type ())
        return make_py_array (value.int32_array_value ());
      else if (value.is_int64_type ())
        return make_py_array (value.int64_array_value ());

      else if (value.is_uint8_type ())
        return make_py_array (value.uint8_array_value ());
      else if (value.is_uint16_type ())
        return make_py_array (value.uint16_array_value ());
      else if (value.is_uint32_type ())
        return make_py_array (value.uint32_array_value ());
      else if (value.is_uint64_type ())
        return make_py_array (value.uint64_array_value ());
    }

  throw value_convert_exception ("unhandled Octave numeric vector type");
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

inline octave_value
wrap_pyobj_to_octvalue (PyObject *obj)
{
  boost::python::object objref { boost::python::handle<> (boost::python::borrowed (obj)) };
  octave_value value;
  pyobj_to_octvalue (value, objref);
  return value;
}

octave_scalar_map
extract_py_scalar_map (PyObject *obj)
{
  if (! obj)
    throw object_convert_exception ("failed to extract map: null object");

  if (! PyDict_Check (obj))
    throw object_convert_exception ("failed to extract map: wrong type");

  octave_scalar_map map;

  Py_ssize_t pos = 0;
  PyObject *py_key = 0;
  PyObject *py_value = 0;

  while (PyDict_Next (obj, &pos, &py_key, &py_value))
    {
      if (! PyBytes_Check (py_key) && ! PyUnicode_Check (py_key))
        throw object_convert_exception ("failed to extract map: bad key type");

      std::string key = extract_py_str (py_key);
      octave_value value = wrap_pyobj_to_octvalue (py_value);
      map.setfield (key, value);
    }

  return map;
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
    {
      int overflow = 0;
#if (defined (HAVE_LONG_LONG) && (SIZEOF_LONG_LONG == 8))
      PY_LONG_LONG value = PyLong_AsLongLongAndOverflow (obj, &overflow);
#else
      long value = PyLong_AsLongAndOverflow (obj, &overflow);
#endif
      if (overflow)
        if (overflow > 0)
          value = std::numeric_limits<int64_t>::max ();
        else
          value = std::numeric_limits<int64_t>::min ();
      return static_cast<int64_t> (value);
    }
#if PY_VERSION_HEX < 0x03000000
  else if (PyInt_Check (obj))
    return PyInt_AsLong (obj);
#endif
  else
    throw object_convert_exception ("failed to extract integer: wrong type");

  return 0;
}

uint64_t
extract_py_uint64 (PyObject *obj)
{
  if (! obj)
    throw object_convert_exception ("failed to extract integer: null object");

  if (PyLong_Check (obj))
    {
      // FIXME: if (value < 0), may be very implementation dependent
      if (Py_SIZE (obj) < 0)
        return 0;

#if (defined (HAVE_LONG_LONG) && (SIZEOF_LONG_LONG == 8))
      unsigned PY_LONG_LONG value = PyLong_AsUnsignedLongLong (obj);
      bool overflow = (value == static_cast<unsigned PY_LONG_LONG> (-1));
#else
      unsigned long value = PyLong_AsUnsignedLong (obj);
      bool overflow = (value == static_cast<unsigned long> (-1));
#endif
      if (overflow)
        {
          value = std::numeric_limits<uint64_t>::max ();
          PyErr_Clear ();
        }

      return static_cast<uint64_t> (value);
    }
#if PY_VERSION_HEX < 0x03000000
  else if (PyInt_Check (obj))
    return static_cast<uint64_t> (PyInt_AsLong (obj));
#endif
  else
    throw object_convert_exception ("failed to extract integer: wrong type");

  return 0;
}

PyObject *
make_py_tuple (const Cell& cell)
{
  if (! (cell.is_empty () || cell.is_vector ()))
    throw value_convert_exception (
      "unable to convert multidimensional cell array into Python tuple");

  octave_idx_type size = cell.numel ();
  PyObject *tuple = PyTuple_New (size);
  if (! tuple)
    octave_throw_bad_alloc ();

  for (octave_idx_type i = 0; i < size; ++i)
    {
      PyObject *item = wrap_octvalue_to_pyobj (cell.xelem (i));
      PyTuple_SET_ITEM (tuple, i, item);
    }

  return tuple;
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
