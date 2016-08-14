/*

Copyright (C) 2015-2016 Mike Miller
Copyright (C) 2008 David Grundberg, Håkan Fors Nilsson
Copyright (C) 2009 VZLU Prague

This file is part of Pytave.

Pytave is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
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

#include <boost/python.hpp>

#include <octave/oct.h>
#include <octave/ov.h>
#include <octave/oct-map.h>
#include <octave/parse.h>

#include <iostream>
#include "arrayobjectdefs.h"
#include "exceptions.h"
#include "octave_to_python.h"
#include "oct-py-types.h"

using namespace boost::python;

namespace pytave
{

  template <class PythonPrimitive, class OctaveBase>
  static void
  copy_octarray_to_pyarrobj (PyArrayObject *pyarr, const OctaveBase& matrix,
                             const unsigned int matindex,
                             const unsigned int matstride,
                             const int dimension, const unsigned int offset)
  {
    unsigned char *ptr = (unsigned char*) PyArray_DATA (pyarr);
    if (dimension == PyArray_NDIM (pyarr) - 1)
      {
        // Last dimension, base case
        for (int i = 0; i < PyArray_DIM (pyarr, dimension); i++)
          {
            *(PythonPrimitive *)&ptr[offset + i*PyArray_STRIDE (pyarr, dimension)]
                = matrix.elem (matindex + i*matstride);
          }
      }
    else
      {
        for (int i = 0; i < PyArray_DIM (pyarr, dimension); i++)
          {
            copy_octarray_to_pyarrobj<PythonPrimitive, OctaveBase> (
              pyarr,
              matrix,
              matindex + i*matstride,
              matstride * PyArray_DIM (pyarr, dimension),
              dimension + 1,
              offset + i*PyArray_STRIDE (pyarr, dimension));
          }
      }
  }

  static PyArrayObject *
  createPyArr (const dim_vector& dims, int pyarrtype)
  {
    int len = dims.length ();
    npy_intp dimensions[len];
    for (int i = 0; i < dims.length (); i++)
      dimensions[i] = dims(i);

    return (PyArrayObject *)PyArray_SimpleNew (len, dimensions, pyarrtype);
  }

  template <class PythonPrimitive, class OctaveBase>
  static PyArrayObject *
  create_array (const OctaveBase& octarr, int pyarraytype)
  {
    PyArrayObject *pyarr = createPyArr (octarr.dims (), pyarraytype);
    try
      {
        copy_octarray_to_pyarrobj<PythonPrimitive, OctaveBase> (pyarr, octarr, 0, 1, 0, 0);
      }
    catch (const value_convert_exception&)
      {
        Py_DECREF (pyarr);
        throw;
      }
    return pyarr;
  }

  static PyArrayObject *
  octvalue_to_pyarrobj (const octave_value& matrix)
  {
    if (matrix.is_double_type ())
      if (matrix.is_complex_type ())
        return create_array<Complex, ComplexNDArray> (matrix.complex_array_value (), NPY_CDOUBLE);
      else if (matrix.is_real_type ())
        return create_array<double, NDArray> (matrix.array_value (), NPY_DOUBLE);
      else
        throw value_convert_exception ("Unknown double matrix type");

    if (matrix.is_single_type ())
      if (matrix.is_complex_type ())
        return create_array<FloatComplex, FloatComplexNDArray> (matrix.float_complex_array_value (), NPY_CFLOAT);
      else if (matrix.is_real_type ())
        return create_array<float, FloatNDArray> (matrix.float_array_value (), NPY_FLOAT);
      else
        throw value_convert_exception ("Unknown float matrix type");

    if (matrix.is_int8_type ())
      return create_array<int8_t, int8NDArray> (matrix.int8_array_value (), NPY_INT8);
    if (matrix.is_int16_type ())
      return create_array<int16_t, int16NDArray> (matrix.int16_array_value (), NPY_INT16);
    if (matrix.is_int32_type ())
      return create_array<int32_t, int32NDArray> (matrix.int32_array_value (), NPY_INT32);
    if (matrix.is_int64_type ())
      return create_array<int64_t, int64NDArray> (matrix.int64_array_value (), NPY_INT64);

    if (matrix.is_uint8_type ())
      return create_array<uint8_t, uint8NDArray> (matrix.uint8_array_value (), NPY_UINT8);
    if (matrix.is_uint16_type ())
      return create_array<uint16_t, uint16NDArray> (matrix.uint16_array_value (), NPY_UINT16);
    if (matrix.is_uint32_type ())
      return create_array<uint32_t, uint32NDArray> (matrix.uint32_array_value (), NPY_UINT32);
    if (matrix.is_uint64_type ())
      return create_array<uint64_t, uint64NDArray> (matrix.uint64_array_value (), NPY_UINT64);

    if (matrix.is_bool_type ())
      return create_array<bool, boolNDArray> (matrix.bool_array_value (), NPY_BOOL);
    if (matrix.is_string ())
      return create_array<char, charNDArray> (matrix.char_array_value (), NPY_CHAR);

    throw value_convert_exception ("Octave matrix type not known, conversion not implemented");
  }

  static void
  octvalue_to_pyarr (boost::python::object& py_object,
                     const octave_value& octvalue)
  {
    PyArrayObject *pyarr = octvalue_to_pyarrobj (octvalue);
    py_object = object (handle<PyObject> ((PyObject *)pyarr));
  }

  static void
  octcell_to_pyobject (boost::python::object& py_object,
                       const Cell& cell)
  {
    if (! (cell.is_empty () || cell.is_vector ()))
      throw value_convert_exception (
        "unable to convert multidimensional cell array into Python sequence");

    boost::python::list sequence;

    for (octave_idx_type i = 0; i < cell.numel (); i++)
      {
        boost::python::object py_val;
        octvalue_to_pyobj (py_val, cell(i));
        sequence.append (py_val);
      }

    py_object = sequence;
  }

  inline PyObject *
  python_integer_value (int32_t value)
  {
#if PY_VERSION_HEX >= 0x03000000
    return PyLong_FromLong (value);
#else
    return PyInt_FromLong (value);
#endif
  }

  inline PyObject *
  python_integer_value (uint32_t value)
  {
    return PyLong_FromUnsignedLong (value);
  }

  inline PyObject *
  python_integer_value (int64_t value)
  {
#if (defined (HAVE_LONG_LONG) && (SIZEOF_LONG_LONG > SIZEOF_LONG))
    return PyLong_FromLongLong (value);
#else
    return PyLong_FromLong (value);
#endif
  }

  inline PyObject *
  python_integer_value (uint64_t value)
  {
#if (defined (HAVE_LONG_LONG) && (SIZEOF_LONG_LONG > SIZEOF_LONG))
    return PyLong_FromUnsignedLongLong (value);
#else
    return PyLong_FromUnsignedLong (value);
#endif
  }

  static void
  octscalar_to_pyobject (boost::python::object& py_object,
                         const octave_value& arg)
  {
    PyObject *obj = 0;

    if (arg.is_complex_type ())
      obj = PyComplex_FromDoubles (arg.real ().double_value (),
                                   arg.imag ().double_value ());
    else if (arg.is_float_type ())
      obj = PyFloat_FromDouble (arg.double_value ());

    else if (arg.is_int8_type ())
      obj = python_integer_value (arg.int8_scalar_value ().value ());
    else if (arg.is_int16_type ())
      obj = python_integer_value (arg.int16_scalar_value ().value ());
    else if (arg.is_int32_type ())
      obj = python_integer_value (arg.int32_scalar_value ().value ());
    else if (arg.is_int64_type ())
      obj = python_integer_value (arg.int64_scalar_value ().value ());

    else if (arg.is_uint8_type ())
      obj = python_integer_value (arg.uint8_scalar_value ().value ());
    else if (arg.is_uint16_type ())
      obj = python_integer_value (arg.uint16_scalar_value ().value ());
    else if (arg.is_uint32_type ())
      obj = python_integer_value (arg.uint32_scalar_value ().value ());
    else if (arg.is_uint64_type ())
      obj = python_integer_value (arg.uint64_scalar_value ().value ());

    else if (arg.is_bool_type ())
      obj = PyBool_FromLong (arg.bool_value ());

    if (obj)
      py_object = object (handle<PyObject> (obj));
    else
      throw value_convert_exception ("unhandled scalar type");
  }

  void octvalue_to_pyobj (boost::python::object& py_object,
                          const octave_value& octvalue)
  {
    if (octvalue.is_undefined ())
      throw value_convert_exception (
        "Octave value `undefined'. Can not convert to a Python object");
    else if (octvalue.is_string ())
      {
        PyObject *obj = make_py_str (octvalue.string_value ());
        py_object = object (handle<PyObject> (obj));
      }
    else if (octvalue.is_scalar_type ())
      octscalar_to_pyobject (py_object, octvalue);
    else if (octvalue.is_cell ())
      octcell_to_pyobject (py_object, octvalue.cell_value ());
    else if (octvalue.is_numeric_type () || octvalue.is_string ()
             || octvalue.is_bool_type ())
      octvalue_to_pyarr (py_object, octvalue);
    else if (octvalue.is_map () && octvalue.numel () == 1)
      {
        PyObject *obj = make_py_dict (octvalue.scalar_map_value ());
        py_object = object (handle<PyObject> (obj));
      }
    else if (octvalue.is_object () && octvalue.class_name () == "pyobject")
      {
        octave_value_list tmp = feval ("getid", ovl (octvalue), 1);
        std::string hexid = tmp(0).string_value ();
        // FIXME: added a messy ref to __InOct__ in __main__, find a better way
        py_object = boost::python::import ("__main__").attr ("__InOct__")[hexid];
      }
    else
      throw value_convert_exception (
        "Conversion from Octave value not implemented");
  }

  void octlist_to_pytuple (boost::python::tuple& python_tuple,
                           const octave_value_list& octave_list)
  {
    boost::python::list seq;
    int length = octave_list.length ();

    for (int i = 0; i < length; i++)
      {
        boost::python::object py_object;
        octvalue_to_pyobj (py_object, octave_list(i));
        seq.append (py_object);
      }

    python_tuple = tuple (seq);
  }
}
