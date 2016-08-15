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
#include <string>

class Cell;
class octave_scalar_map;

namespace pytave
{

PyObject *
make_py_dict (const octave_scalar_map& map);

int64_t
extract_py_int64 (PyObject *obj);

PyObject *
make_py_list (const Cell& cell);

std::string
extract_py_str (PyObject *obj);

PyObject *
make_py_str (const std::string& str);

}

#endif
