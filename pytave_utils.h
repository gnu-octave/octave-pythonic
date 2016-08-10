/*

Copyright (C) 2016 Abhinav Tripathi

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

#if ! defined (pytave_utils_h)
#define pytave_utils_h

#include <boost/python.hpp>

namespace pytave
{
  void get_builtins_module (boost::python::object& builtins_module);
  void get_object_from_python (const octave_value& oct_value,
                               boost::python::object& py_object);
}

#endif