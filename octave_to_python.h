/*

Copyright (C) 2008 David Grundberg, Håkan Fors Nilsson

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

#ifndef OCTAVE_TO_PYTHON_H
#define OCTAVE_TO_PYTHON_H

namespace pytave {
   void octvalue_to_pyobj(boost::python::object &py_object,
                          const octave_value& octvalue);
   void octlist_to_pytuple(boost::python::tuple &python_tuple,
                           const octave_value_list &octave_list);
}

#endif /* OCTAVE_TO_PYTHON_H */
