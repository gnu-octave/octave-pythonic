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

#if ! defined (pytave_oct_py_types_h)
#define pytave_oct_py_types_h 1

#include <Python.h>
#include <complex>
#include <string>

class Cell;
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

//! Create a Python complex object with the value of the given @c complex value.
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

//! Create a Python list object from the given Octave cell array value.
//!
//! The values contained in the cell array are recursively converted to
//! appropriate Python values.
//!
//! @param cell Octave cell array
//! @return Python list object
PyObject *
make_py_list (const Cell& cell);

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

//! Extract the string value of the given Python str, bytes, or unicode object.
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

}

#endif
