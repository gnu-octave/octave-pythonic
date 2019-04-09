/*

Copyright (C) 2016-2019 Mike Miller

This file is part of Octave Pythonic.

Octave Pythonic is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Octave Pythonic is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Octave Pythonic; see the file COPYING.  If not, see
<https://www.gnu.org/licenses/>.

*/

#if defined (HAVE_CONFIG_H)
#  include <config.h>
#endif

#include <Python.h>
#include <limits>
#include <octave/Cell.h>
#include <octave/oct-map.h>
#include <octave/quit.h>
#include <octave/ov-null-mat.h>

#include "oct-py-error.h"
#include "oct-py-eval.h"
#include "oct-py-object.h"
#include "oct-py-types.h"
#include "oct-py-util.h"

namespace pytave
{

  PyObject *
  make_py_bool (bool value)
  {
    python_object retval = value ? Py_True : Py_False;
    Py_INCREF (retval);
    return retval.release ();
  }

  PyObject *
  make_py_complex (std::complex<double> value)
  {
    Py_complex py_complex_value {value.real (), value.imag ()};
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
      error_conversion_invalid_python_object ("a boolean value");

    if (! PyBool_Check (obj))
      error_conversion_mismatch_python_type ("a boolean value", "bool");

    return (obj == Py_True);
  }

  std::complex<double>
  extract_py_complex (PyObject *obj)
  {
    if (! obj)
      error_conversion_invalid_python_object ("a complex value");

    if (! PyComplex_Check (obj))
      error_conversion_mismatch_python_type ("a complex value", "complex");

    Py_complex value = PyComplex_AsCComplex (obj);
    return std::complex<double> {value.real, value.imag};
  }

  double
  extract_py_float (PyObject *obj)
  {
    if (! obj)
      error_conversion_invalid_python_object ("a floating point value");

    if (! PyFloat_Check (obj))
      error_conversion_mismatch_python_type ("a floating point value", "float");

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
      error ("unable to create array, invalid array type code");

    std::string arg { typecode };
    python_object array = py_call_function ("array.array", ovl (arg));

    if (len > 0)
      {
        // create a byte buffer containing a copy of the array binary data
        const char *cdata = reinterpret_cast<const char *> (data);
        python_object buf = PyBytes_FromStringAndSize (cdata, len);
        if (! buf)
          octave_throw_bad_alloc ();

        PyObject *frombytes = (PyObject_HasAttrString (array, "frombytes") ?
                               PyObject_GetAttrString (array, "frombytes") :
                               PyObject_GetAttrString (array, "fromstring"));
        python_object args = PyTuple_Pack (1, buf.release ());
        py_call_function (frombytes, args);
      }

    return array.release ();
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
    if (! value.is_scalar_type ())
      error ("unable to convert non-scalar type \"%s\" to a Python number",
             value.type_name ().c_str ());

    if (value.islogical ())
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

    else if (value.iscomplex ())
      return make_py_complex (value.complex_value ());
    else if (value.isfloat ())
      return make_py_float (value.double_value ());
    else
      error ("unable to convert unhandled scalar type \"%s\" to a "
             "Python number", value.type_name ().c_str ());

    return nullptr;
  }

  PyObject *
  make_py_array (const octave_value& value)
  {
    if (! (value.isnumeric () && ! value.iscomplex ()
           && value.ndims () == 2
           && (value.columns () <= 1 || value.rows () <= 1)))
      error ("unable to convert non-vector type \"%s\" to a Python array",
             value.type_name ().c_str ());

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
    else
      error ("unable to convert unhandled vector type \"%s\" to a "
             "Python array", value.type_name ().c_str ());

    return nullptr;
  }

  octave_scalar_map
  extract_py_scalar_map (PyObject *obj)
  {
    if (! obj)
      error_conversion_invalid_python_object ("an Octave struct");

    if (! PyDict_Check (obj))
      error_conversion_mismatch_python_type ("an Octave struct", "dict");

    octave_scalar_map map;

    Py_ssize_t pos = 0;
    PyObject *py_key = nullptr;
    PyObject *py_value = nullptr;

    while (PyDict_Next (obj, &pos, &py_key, &py_value))
      {
        if (! PyBytes_Check (py_key) && ! PyUnicode_Check (py_key))
          error ("unable to convert Python dict to Octave struct, "
                 "all keys in the dict must be strings");

        std::string key = extract_py_str (py_key);
        octave_value value = py_implicitly_convert_return_value (py_value);
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

        PyObject *item = py_implicitly_convert_argument (map.contents (p));

        if (PyDict_SetItem (dict, key, item) < 0)
          error_python_exception ();
      }

    return dict;
  }

  int64_t
  extract_py_int64 (PyObject *obj)
  {
    if (! obj)
      error_conversion_invalid_python_object ("a signed integer value");

    if (PyLong_Check (obj))
      {
        int overflow = 0;
#if (defined (HAVE_LONG_LONG) && (SIZEOF_LONG_LONG == 8))
        PY_LONG_LONG value = PyLong_AsLongLongAndOverflow (obj, &overflow);
#else
        long value = PyLong_AsLongAndOverflow (obj, &overflow);
#endif
        if (overflow)
          {
            if (overflow > 0)
              value = std::numeric_limits<int64_t>::max ();
            else
              value = std::numeric_limits<int64_t>::min ();
          }
        return static_cast<int64_t> (value);
      }
#if PY_VERSION_HEX < 0x03000000
    else if (PyInt_Check (obj))
      return PyInt_AsLong (obj);
#endif
    else
      error_conversion_mismatch_python_type ("a signed integer value",
                                             "int or long");

    return 0;
  }

  uint64_t
  extract_py_uint64 (PyObject *obj)
  {
    if (! obj)
      error_conversion_invalid_python_object ("an unsigned integer value");

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
      error_conversion_mismatch_python_type ("an unsigned integer value",
                                             "int or long");

    return 0;
  }

  PyObject *
  make_py_tuple (const Cell& cell)
  {
    if (! (cell.isempty () || cell.isvector ()))
      error ("unable to convert multidimensional cell array to a Python tuple");

    octave_idx_type size = cell.numel ();
    PyObject *tuple = PyTuple_New (size);
    if (! tuple)
      octave_throw_bad_alloc ();

    for (octave_idx_type i = 0; i < size; ++i)
      {
        PyObject *item = py_implicitly_convert_argument (cell.xelem (i));
        PyTuple_SET_ITEM (tuple, i, item);
      }

    return tuple;
  }

  std::string
  extract_py_str (PyObject *obj)
  {
    std::string retval;

    if (! obj)
      error_conversion_invalid_python_object ("a string value");

    if (PyBytes_Check (obj))
      {
        retval.assign (PyBytes_AsString (obj), PyBytes_Size (obj));
      }
    else if (PyUnicode_Check (obj))
      {
        python_object enc = PyUnicode_AsUTF8String (obj);
        if (enc && PyBytes_Check (enc))
          retval.assign (PyBytes_AsString (enc), PyBytes_Size (enc));
        else
          octave_throw_bad_alloc ();
      }
    else
      error_conversion_mismatch_python_type ("a string value", "str");

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

  PyObject *
  py_implicitly_convert_argument (const octave_value& value)
  {
    if (value.isobject () && value.class_name () == "pyobject")
      return pyobject_unwrap_object (value);
    else if (value.is_string () && value.rows () > 1)
      error ("unable to convert multirow char array to a Python object");
    else if (value.is_string ())
      return make_py_str (value.string_value ());
    else if (value.is_scalar_type ())
      return make_py_numeric_value (value);
    else if (value.iscell ())
      return make_py_tuple (value.cell_value ());
    else if (value.isnumeric () && value.ndims () == 2
             && (value.columns () <= 1 || value.rows () <= 1))
      return make_py_array (value);
    else if (value.isstruct () && value.numel () == 1)
      return make_py_dict (value.scalar_map_value ());
    else
      error ("unable to convert unhandled Octave type to a Python object");

    return nullptr;
  }

  octave_value
  py_implicitly_convert_return_value (PyObject *obj)
  {
    if (PyBool_Check (obj))
      return octave_value {extract_py_bool (obj)};
#if PY_VERSION_HEX < 0x03000000
    else if (PyInt_Check (obj))
      return octave_value {octave_int64 (extract_py_int64 (obj))};
#endif
    else if (PyComplex_Check (obj))
      return octave_value {extract_py_complex (obj)};
    else if (PyFloat_Check (obj))
      return octave_value {extract_py_float (obj)};
    else
      return pyobject_wrap_object (obj);
  }

}
