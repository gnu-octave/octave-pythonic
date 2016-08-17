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

#if ! defined (pytave_oct_py_eval_h)
#define pytave_oct_py_eval_h 1

#include <Python.h>
#include <string>

class octave_value_list;

namespace pytave
{

//! Call a Python function by name with the given argument list.
//!
//! The @a func string may be the name of a builtin function or of a function
//! in a loadable module in the form @c module.function.
//!
//! If @a args contains one or more values created by the @c pyargs function,
//! they are automatically interpreted as keyword arguments.
//!
//! @param func name of a Python function
//! @param args Octave argument list to be converted and passed to @a func
//! @return return value of @a func
PyObject *
py_call_function (const std::string& func, const octave_value_list& args);

//! Call a Python function by name with arguments and keyword arguments.
//!
//! The @a func string may be the name of a builtin function or of a function
//! in a loadable module in the form @c module.function.
//!
//! @param func name of a Python function
//! @param args tuple of positional arguments
//! @param kwargs dictionary of keyword arguments
//! @return return value of @a func
PyObject *
py_call_function (const std::string& func, PyObject *args, PyObject *kwargs = 0);

//! Call a Python function with the given argument list.
//!
//! If @a args contains one or more values created by the @c pyargs function,
//! they are automatically interpreted as keyword arguments.
//!
//! @param callable Python function or other callable object
//! @param args Octave argument list to be converted and passed to @a func
//! @return return value of @a func
PyObject *
py_call_function (PyObject *callable, const octave_value_list& args);

//! Call a Python function with arguments and keyword arguments.
//!
//! @param callable Python function or other callable object
//! @param args tuple of positional arguments
//! @param kwargs dictionary of keyword arguments
//! @return return value of @a func
PyObject *
py_call_function (PyObject *callable, PyObject *args, PyObject *kwargs = 0);

PyObject *
py_eval_string (const std::string& expr, PyObject *globals = 0, PyObject *locals = 0);

PyObject *
py_exec_string (const std::string& expr, PyObject *globals = 0, PyObject *locals = 0);

}

#endif
