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

#include <string>
#include <boost/python.hpp>

#include "oct-py-eval.h"

namespace pytave
{

PyObject *
py_run_string_safe (const std::string& expr, int start, PyObject *globals,
                    PyObject *locals)
{
  bool alloc = false;

  if (! globals || (globals == Py_None))
    {
      globals = PyEval_GetGlobals ();
      if (! globals)
        {
          globals = PyDict_New ();
          alloc = true;
        }
    }

  if (! locals || (locals == Py_None))
    locals = globals;

  // Evaluate all expressions under "from __future__ import print_function"
  PyCompilerFlags flags { CO_FUTURE_PRINT_FUNCTION };

  PyObject *retval = PyRun_StringFlags (expr.c_str (), start, globals, locals,
                                        &flags);

  if (alloc)
    Py_DECREF (globals);

  if (! retval)
    throw boost::python::error_already_set ();

  return retval;
}

PyObject *
py_eval_string (const std::string& expr, PyObject *globals, PyObject *locals)
{
  return py_run_string_safe (expr, Py_eval_input, globals, locals);
}

PyObject *
py_exec_string (const std::string& expr, PyObject *globals, PyObject *locals)
{
  return py_run_string_safe (expr, Py_file_input, globals, locals);
}

}
