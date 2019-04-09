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

#if ! defined (pytave_oct_py_types_h)
#define pytave_oct_py_types_h 1

#include <Python.h>
#include <complex>
#include <string>

class Cell;
class FloatNDArray;
class NDArray;
template <typename T> class intNDArray;
class octave_scalar_map;
class octave_value;

namespace pytave
{

  //! Extract the integer value of the given Python bool object.
  //!
  //! @param obj Python bool object
  //! @return @c true or @c false value of @a obj
  bool
  extract_py_bool (PyObject *obj);

  //! Create a Python bool object with the value of the given @c bool value.
  //!
  //! @param value @c true or @c false value
  //! @return Python bool object
  PyObject *
  make_py_bool (bool value);

  //! Extract the complex value of the given Python complex object.
  //!
  //! @param obj Python complex object
  //! @return complex value of @a obj
  std::complex<double>
  extract_py_complex (PyObject *obj);

  //! Create a Python complex object with the value of the given @c complex
  //! value.
  //!
  //! @param value complex value
  //! @return Python complex object
  PyObject *
  make_py_complex (std::complex<double> value);

  //! Extract the floating point value of the given Python float object.
  //!
  //! @param obj Python float object
  //! @return floating point value of @a obj
  double
  extract_py_float (PyObject *obj);

  //! Create a Python float object with the value of the given @c double value.
  //!
  //! @param value floating point value
  //! @return Python float object
  PyObject *
  make_py_float (double value);

  //! Extract an Octave scalar map from the given Python dict object.
  //!
  //! @param obj Python dict object
  //! @return Octave scalar map containing the items of @a obj
  octave_scalar_map
  extract_py_scalar_map (PyObject *obj);

  //! Create a Python dict object from the given Octave scalar map value.
  //!
  //! The values contained in the map are recursively converted to appropriate
  //! Python values.
  //!
  //! @param map Octave scalar map
  //! @return Python dict object
  PyObject *
  make_py_dict (const octave_scalar_map& map);

  //! Extract the integer value of the given Python int or long object.
  //!
  //! @param obj Python int or long object
  //! @return integer value of @a obj
  int64_t
  extract_py_int64 (PyObject *obj);

  //! Extract the integer value of the given Python int or long object.
  //!
  //! @param obj Python int or long object
  //! @return integer value of @a obj
  uint64_t
  extract_py_uint64 (PyObject *obj);

  //! Create a Python int object with the value of the given @c int32_t value.
  //!
  //! @param value integer value
  //! @return Python int or long object
  PyObject *
  make_py_int (int32_t value);

  //! Create a Python int object with the value of the given @c uint32_t value.
  //!
  //! @param value integer value
  //! @return Python int or long object
  PyObject *
  make_py_int (uint32_t value);

  //! Create a Python int object with the value of the given @c int64_t value.
  //!
  //! @param value integer value
  //! @return Python int or long object
  PyObject *
  make_py_int (int64_t value);

  //! Create a Python int object with the value of the given @c uint64_t value.
  //!
  //! @param value integer value
  //! @return Python int or long object
  PyObject *
  make_py_int (uint64_t value);

  //! Create a Python array object with the value of the given Octave array.
  //!
  //! @param nda array value
  //! @return Python array object
  PyObject *
  make_py_array (const NDArray& nda);

  //! Create a Python array object with the value of the given Octave array.
  //!
  //! @param nda array value
  //! @return Python array object
  PyObject *
  make_py_array (const FloatNDArray& nda);

  //! Create a Python array object with the value of the given Octave array.
  //!
  //! @param nda array value
  //! @return Python array object
  template <typename T>
  PyObject *
  make_py_array (const intNDArray<T>& nda);

  //! Create a Python array object from the given Octave numeric vector.
  //!
  //! All Octave real floating point and integer values are converted to
  //! corresponding Python array types by this function.
  //!
  //! @warning Depending on the version of Python and how it is configured,
  //!          @c int64 and @c uint64 vectors may not be supported.
  //!
  //! @param value Octave numeric or boolean scalar value
  //! @return Python array object
  PyObject *
  make_py_array (const octave_value& value);

  //! Create a Python tuple object from the given Octave cell array value.
  //!
  //! The values contained in the cell array are recursively converted to
  //! appropriate Python values.
  //!
  //! @param cell Octave cell array
  //! @return Python tuple object
  PyObject *
  make_py_tuple (const Cell& cell);

  //! Create a Python numeric object from the given Octave numeric or boolean
  //! scalar value.
  //!
  //! The following implicit type conversions are implemented by this function:
  //!
  //! @arg @c bool from Octave logical scalar,
  //! @arg @c complex from Octave double or single precision complex scalar,
  //! @arg @c float from Octave double or single precision scalar,
  //! @arg @c int from any Octave integer-valued scalar,
  //! @arg @c long from any Octave @c uint32, @c int64, or @c uint64, and only
  //!         if running against Python 2.
  //!
  //! @param value Octave numeric or boolean scalar value
  //! @return Python numeric object (@c bool, @c int, @c long, @c float, or
  //!         @c complex)
  PyObject *
  make_py_numeric_value (const octave_value& value);

  //! Extract the string value of the given Python str, bytes, or unicode
  //! object.
  //!
  //! @param obj Python str, bytes, or unicode object
  //! @return string value of @a obj
  std::string
  extract_py_str (PyObject *obj);

  //! Create a Python str object from the given @c string value.
  //!
  //! @param str string value
  //! @return Python str object
  PyObject *
  make_py_str (const std::string& str);

  //! Perform an implicit conversion of the given Octave @c value to a Python
  //! argument.
  //!
  //! The following implicit type conversions are implemented by this function:
  //!
  //! @arg @c bool from Octave logical scalar,
  //! @arg @c complex from Octave double or single precision complex scalar,
  //! @arg @c float from Octave double or single precision scalar,
  //! @arg @c int from any Octave integer-valued scalar,
  //! @arg @c long from Octave @c uint32, @c int64, or @c uint64, and only if
  //!         running against Python 2,
  //! @arg @c str from Octave string (@c char row vector),
  //! @arg @c array.array from Octave numeric column or row vector,
  //! @arg @c dict from Octave scalar map (consisting entirely of implicitly
  //!         convertible elements),
  //! @arg @c tuple from Octave cell array (consisting entirely of implicitly
  //!         convertible elements).
  //!
  //! If @c value refers to a previously created Python object, then a reference
  //! to the existing object is returned.
  //!
  //! Otherwise, a conversion error is raised.
  //!
  //! @param value Octave value
  //! @return Python object
  PyObject *
  py_implicitly_convert_argument (const octave_value& value);

  //! Perform an implicit conversion of the given Python object to an Octave
  //! return value.
  //!
  //! The following implicit type conversions are implemented by this function:
  //!
  //! @arg @c logical scalar from Python @c bool,
  //! @arg @c complex @c double from Python @c complex,
  //! @arg @c double from Python @c float,
  //! @arg @c int64 from Python @c int, only if running against Python 2.
  //!
  //! Otherwise, @c obj is left unconverted, a reference is maintained to it,
  //! and an Octave value containing that reference is returned.
  //!
  //! @param obj Python object
  //! @return Octave value
  octave_value
  py_implicitly_convert_return_value (PyObject *obj);

}

#endif
