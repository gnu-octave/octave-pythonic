/*

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

#include <iostream>
#include <boost/python.hpp>
#include <boost/python/numeric.hpp>
#include <boost/type_traits/integral_constant.hpp>

#include <octave/oct.h>
#include <octave/oct-map.h>
#include <octave/Cell.h>
#include <octave/ov.h>
#include <octave/Array.h>

#include "arrayobjectdefs.h"
#include "exceptions.h"
#include "python_to_octave.h"

using namespace boost::python;

namespace pytave
{
  template <class PythonPrimitive, class OctaveBase>
  static void
  copy_pyarrobj_to_octarray (OctaveBase& matrix, PyArrayObject *pyarr,
                             const int unsigned matindex,
                             const unsigned int matstride,
                             const int dimension, const unsigned int offset)
  {
    unsigned char *ptr = (unsigned char*) PyArray_DATA (pyarr);
    if (dimension == PyArray_NDIM (pyarr) - 1)
      {
        // Last dimension, base case
        for (int i = 0; i < PyArray_DIM (pyarr, dimension); i++)
          {
            matrix.elem (matindex + i*matstride)
               = *(PythonPrimitive*)
               &ptr[offset + i*PyArray_STRIDE (pyarr, dimension)];
          }
      }
    else if (PyArray_NDIM (pyarr) == 0)
      {
        matrix.elem (0) = *(PythonPrimitive*) ptr;
      }
    else
      {
        for (int i = 0; i < PyArray_DIM (pyarr, dimension); i++)
          {
            copy_pyarrobj_to_octarray<PythonPrimitive, OctaveBase> (
              matrix,
              pyarr,
              matindex + i*matstride,
              matstride * PyArray_DIM (pyarr, dimension),
              dimension + 1,
              offset + i*PyArray_STRIDE (pyarr, dimension));
          }
      }
  }

  template <>
  void
  copy_pyarrobj_to_octarray<PyObject *, Cell> (Cell& matrix,
                                               PyArrayObject *pyarr,
                                               const int unsigned matindex,
                                               const unsigned int matstride,
                                               const int dimension,
                                               const unsigned int offset)
  {
    unsigned char *ptr = (unsigned char*) PyArray_DATA (pyarr);
    if (dimension == PyArray_NDIM (pyarr) - 1)
      {
        // Last dimension, base case
        for (int i = 0; i < PyArray_DIM (pyarr, dimension); i++)
          {
            PyObject *pobj = *(PyObject **)
               &ptr[offset + i*PyArray_STRIDE (pyarr, dimension)];
            pyobj_to_octvalue (matrix.elem (matindex + i*matstride),
                               object (handle<PyObject> (borrowed (pobj))));
          }
      }
    else if (PyArray_NDIM (pyarr) == 0)
      {
        PyObject *pobj = *(PyObject **) ptr;
        pyobj_to_octvalue (matrix.elem (0),
                           object (handle<PyObject> (borrowed (pobj))));
      }
    else
      {
        for (int i = 0; i < PyArray_DIM (pyarr, dimension); i++)
          {
            copy_pyarrobj_to_octarray<PyObject *, Cell> (
              matrix,
              pyarr,
              matindex + i*matstride,
              matstride * PyArray_DIM (pyarr, dimension),
              dimension + 1,
              offset + i*PyArray_STRIDE (pyarr, dimension));
          }
      }
  }

  template <class PythonPrimitive, class OctaveBase>
  static void
  copy_pyarrobj_to_octarray_dispatch (OctaveBase& matrix,
                                      PyArrayObject *pyarr,
                                      const boost::true_type&)
  {
    copy_pyarrobj_to_octarray<PythonPrimitive, OctaveBase> (matrix, pyarr, 0, 1, 0, 0);
  }

  template <class PythonPrimitive, class OctaveBase>
  static void
  copy_pyarrobj_to_octarray_dispatch (OctaveBase& matrix,
                                      PyArrayObject *pyarr,
                                      const boost::false_type&)
  {
    assert (0);
  }

  template <class X, class Y> class matching_type : public boost::false_type { };
  template <class X> class matching_type<X, X> : public boost::true_type { };
  template <class X> class matching_type<X, octave_int<X> > : public boost::true_type { };
  template <> class matching_type<float, double> : public boost::true_type { };
  template <> class matching_type<FloatComplex, Complex> : public boost::true_type { };
  template <> class matching_type<PyObject *, octave_value> : public boost::true_type { };

  template <class PythonPrimitive, class OctaveBase>
  static void
  copy_pyarrobj_to_octarray_dispatch (OctaveBase& matrix,
                                       PyArrayObject *pyarr)
  {
    matching_type<PythonPrimitive, typename OctaveBase::element_type> inst;
    copy_pyarrobj_to_octarray_dispatch<PythonPrimitive, OctaveBase> (matrix, pyarr, inst);
  }

  template <class OctaveBase>
  static void
  copy_pyarrobj_to_octarray_boot (OctaveBase& matrix, PyArrayObject *pyarr)
  {

#define ARRAYCASE(AC_pyarrtype, AC_primitive) case AC_pyarrtype: \
         copy_pyarrobj_to_octarray_dispatch<AC_primitive, OctaveBase> \
         (matrix, pyarr); \
         break; \

    // Coerce NumPy's long type into one of two possible sized integer types
    int type_num = PyArray_TYPE (pyarr);
    switch (type_num)
      {
      case NPY_LONG:
        if (sizeof (npy_long) == sizeof (int64_t))
          type_num = NPY_INT64;
        else if (sizeof (npy_long) == sizeof (int32_t))
          type_num = NPY_INT32;
        break;
      case NPY_LONGLONG:
        if (sizeof (npy_longlong) == sizeof (int64_t))
          type_num = NPY_INT64;
        else if (sizeof (npy_longlong) == sizeof (int32_t))
          type_num = NPY_INT32;
        break;
      case NPY_ULONG:
        if (sizeof (npy_ulong) == sizeof (uint64_t))
          type_num = NPY_UINT64;
        else if (sizeof (npy_ulong) == sizeof (uint32_t))
          type_num = NPY_UINT32;
        break;
      case NPY_ULONGLONG:
        if (sizeof (npy_ulonglong) == sizeof (uint64_t))
          type_num = NPY_UINT64;
        else if (sizeof (npy_ulonglong) == sizeof (uint32_t))
          type_num = NPY_UINT32;
        break;
      }

    switch (type_num)
      {
      ARRAYCASE (NPY_INT8,    int8_t)
      ARRAYCASE (NPY_INT16,   int16_t)
      ARRAYCASE (NPY_INT32,   int32_t)
      ARRAYCASE (NPY_INT64,   int64_t)
      ARRAYCASE (NPY_UINT8,   uint8_t)
      ARRAYCASE (NPY_UINT16,  uint16_t)
      ARRAYCASE (NPY_UINT32,  uint32_t)
      ARRAYCASE (NPY_UINT64,  uint64_t)
      ARRAYCASE (NPY_FLOAT,   float)
      ARRAYCASE (NPY_DOUBLE,  double)
      ARRAYCASE (NPY_CFLOAT,  FloatComplex)
      ARRAYCASE (NPY_CDOUBLE, Complex)
      ARRAYCASE (NPY_BOOL,    bool)
      ARRAYCASE (NPY_CHAR,    char)
      ARRAYCASE (NPY_STRING,  char)
      ARRAYCASE (NPY_OBJECT,  PyObject *)

      default:
        throw object_convert_exception (
          PyEval_GetFuncName ((PyObject*)pyarr)
          + (PyEval_GetFuncDesc ((PyObject*)pyarr)
          + std::string (": Unsupported Python array type")));
      }
  }

  template <class OctaveBase>
  static void
  pyarrobj_to_octvalueNd (octave_value& octvalue, PyArrayObject *pyarr,
                          const dim_vector& dims)
  {
    OctaveBase array (dims);
    copy_pyarrobj_to_octarray_boot<OctaveBase> (array, pyarr);
    octvalue = array;
  }

  static void
  pyarr_to_octvalue (octave_value& octvalue, PyArrayObject *pyarr)
  {
    dim_vector dims;
    switch (PyArray_NDIM (pyarr))
      {
      case 0:
        dims = dim_vector (1, 1);
        break;
      case 1:
        // Always make PyArray vectors row vectors.
        dims = dim_vector (1, PyArray_DIM (pyarr, 0));
        break;
      default:
        dims.resize (PyArray_NDIM (pyarr));
        for (int d = 0; d < PyArray_NDIM (pyarr); d++)
          dims(d) = PyArray_DIM (pyarr, d);
        break;
      }

    switch (PyArray_TYPE (pyarr))
      {
      case NPY_BYTE:
      case NPY_SHORT:
      case NPY_INT:
      case NPY_LONG:
      case NPY_LONGLONG:
        switch (PyArray_ITEMSIZE (pyarr))
          {
          case 1:
            pyarrobj_to_octvalueNd<int8NDArray> (octvalue, pyarr, dims);
            break;
          case 2:
            pyarrobj_to_octvalueNd<int16NDArray> (octvalue, pyarr, dims);
            break;
          case 4:
            pyarrobj_to_octvalueNd<int32NDArray> (octvalue, pyarr, dims);
            break;
          case 8:
            pyarrobj_to_octvalueNd<int64NDArray> (octvalue, pyarr, dims);
            break;
          default:
            throw object_convert_exception ("Unknown integer.");
          }
        break;
      case NPY_UBYTE:
      case NPY_USHORT:
      case NPY_UINT:
      case NPY_ULONG:
      case NPY_ULONGLONG:
        switch (PyArray_ITEMSIZE (pyarr))
          {
          case 1:
            pyarrobj_to_octvalueNd<uint8NDArray> (octvalue, pyarr, dims);
            break;
          case 2:
            pyarrobj_to_octvalueNd<uint16NDArray> (octvalue, pyarr, dims);
            break;
          case 4:
            pyarrobj_to_octvalueNd<uint32NDArray> (octvalue, pyarr, dims);
            break;
          case 8:
            pyarrobj_to_octvalueNd<uint64NDArray> (octvalue, pyarr, dims);
            break;
          default:
            throw object_convert_exception ("Unknown unsigned integer.");
          }
        break;
      case NPY_FLOAT:
        pyarrobj_to_octvalueNd<FloatNDArray> (octvalue, pyarr, dims);
        break;
      case NPY_DOUBLE:
        pyarrobj_to_octvalueNd<NDArray> (octvalue, pyarr, dims);
        break;
      case NPY_CFLOAT:
        pyarrobj_to_octvalueNd<FloatComplexNDArray> (octvalue, pyarr, dims);
        break;
      case NPY_CDOUBLE:
        pyarrobj_to_octvalueNd<ComplexNDArray> (octvalue, pyarr, dims);
        break;
      case NPY_BOOL:
        pyarrobj_to_octvalueNd<boolNDArray> (octvalue, pyarr, dims);
        break;
      case NPY_CHAR:
      case_NPY_CHAR:
        pyarrobj_to_octvalueNd<charNDArray> (octvalue, pyarr, dims);
        // FIXME: is the following needed?
        octvalue = octvalue.convert_to_str (true, true, '"');
        break;
      case NPY_STRING:
        {
          if (PyArray_ITEMSIZE (pyarr) == 1)
            goto case_NPY_CHAR;
          else
            {
              // Create a new descriptor of the data.
              PyArray_Descr *view_descr = PyArray_DescrFromType (NPY_CHAR);
              // Create a new view of the NumPy array.
              PyArrayObject *view = (PyArrayObject *)PyArray_View (pyarr, view_descr, 0);
              // Store in a handle to ensure proper destruction.
              handle<PyObject> view_handle (allow_null ((PyObject *)view));
              // Call recursively.
              pyarr_to_octvalue (octvalue, view);
            }
        }
        break;
      case NPY_OBJECT:
        pyarrobj_to_octvalueNd<Cell> (octvalue, pyarr, dims);
        break;
      default:
        throw object_convert_exception (
          PyEval_GetFuncDesc ((PyObject*)(pyarr)) + std::string (" ")
          + PyEval_GetFuncName ((PyObject*)(pyarr))
          + ": Encountered unsupported Python array");
        break;
      }
  }

  static void
  pylist_to_cellarray (octave_value& oct_value, const boost::python::list& list)
  {
    octave_idx_type length = boost::python::extract<octave_idx_type> (list.attr ("__len__") ());
    octave_value_list values;

    for (octave_idx_type i = 0; i < length; i++)
      {
         octave_value val;

         pyobj_to_octvalue (val, list[i]);
         values.append (val);

      }

    oct_value = Cell (values);
  }

  static void
  pydict_to_octmap (octave_value& oct_value, const boost::python::dict& dict)
  {
    boost::python::list list = dict.items ();
    octave_idx_type length = boost::python::extract<octave_idx_type> (list.attr ("__len__") ());

    dim_vector dims = dim_vector (1, 1);

    octave_value_list vals (length);
    string_vector keys (length);

    // Extract all keys and convert values. Remember whether dimensions
    // match.

    for (octave_idx_type i = 0; i < length; i++)
      {
        std::string& key = keys[i];

        boost::python::tuple tuple =
            boost::python::extract<boost::python::tuple> (list[i]) ();

        boost::python::extract<std::string> str (tuple[0]);
        if (! str.check ())
          throw object_convert_exception (
            std::string ("Can not convert key of type ")
            + PyEval_GetFuncName (boost::python::object (tuple[0]).ptr ())
            + PyEval_GetFuncDesc (boost::python::object (tuple[0]).ptr ())
            + " to a structure field name. Field names must be strings.");

        key = str ();

        if (! valid_identifier (key))
          throw object_convert_exception (
            std::string ("Can not convert key `") + key + "' to a structure "
            "field name. Field names must be valid Octave identifiers.");

        octave_value& val = vals(i);

        pyobj_to_octvalue (val, tuple[1]);

        if (val.is_cell ())
          {
            if (i == 0)
               dims = val.dims ();
            else if (val.numel () != 1 && val.dims () != dims)
               throw object_convert_exception (
                 "Dimensions of the struct fields do not match");
          }
      }

    octave_map map = octave_map (dims);

    for (octave_idx_type i = 0; i < length; i++)
      {
        std::string& key = keys[i];
        octave_value val = vals(i);

        if (val.is_cell ())
         {
            const Cell c = val.cell_value ();
            if (c.numel () == 1)
            {
               map.assign (key, Cell (dims, c(0)));
            }
            else
            {
               map.assign (key, c);
            }
         }
        else
          map.assign (key, Cell (dims, val));
      }
    oct_value = map;
  }

  void pyobj_to_octvalue (octave_value& oct_value,
                          const boost::python::object& py_object)
  {
    extract<int> intx (py_object);
    extract<double> doublex (py_object);
    extract<Complex> complexx (py_object);
    extract<std::string> stringx (py_object);
    extract<numeric::array> arrayx (py_object);
    extract<boost::python::list> listx (py_object);
    extract<boost::python::dict> dictx (py_object);

    if (intx.check ())
      oct_value = intx ();
    else if (doublex.check ())
      oct_value = doublex ();
    else if (complexx.check ())
      oct_value = complexx ();
    else if (arrayx.check ())
      pyarr_to_octvalue (oct_value, (PyArrayObject*)py_object.ptr ());
    else if (stringx.check ())
      oct_value = stringx ();
    else if (listx.check ())
      pylist_to_cellarray (oct_value, (boost::python::list&)py_object);
    else if (dictx.check ())
      pydict_to_octmap (oct_value, (boost::python::dict&)py_object);
    else
      throw object_convert_exception (
        PyEval_GetFuncName (py_object.ptr ())
        + (PyEval_GetFuncDesc (py_object.ptr ())
        + std::string (": Unsupported Python object type, "
                       "cannot convert to Octave value")));
  }

  void pytuple_to_octlist (octave_value_list& octave_list,
                           const boost::python::tuple& python_tuple)
  {
    int length = extract<int> (python_tuple.attr ("__len__") ());

    for (int i = 0; i < length; i++)
      {
        pyobj_to_octvalue (octave_list(i), python_tuple[i]);
      }
  }
}
