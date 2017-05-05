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

#include <Python.h>
#include <string>
#include <octave/ov.h>
#include <octave/ovl.h>

#include "oct-py-error.h"
#include "oct-py-eval.h"
#include "oct-py-object.h"
#include "oct-py-util.h"
#include "oct-py-types.h"

namespace pytave
{

  PyObject *
  py_call_function (const std::string& func, const octave_value_list& args)
  {
    python_object func_obj = py_find_function (func);
    python_object retval = py_call_function (func_obj, args);
    return retval.release ();
  }

  PyObject *
  py_call_function (const std::string& func, PyObject *args, PyObject *kwargs)
  {
    python_object func_obj = py_find_function (func);
    python_object retval = py_call_function (func_obj, args, kwargs);
    return retval.release ();
  }

  PyObject *
  py_call_function (PyObject *callable, const octave_value_list& args)
  {
    python_object kwargs;
    python_object args_list = PyList_New (0);
    if (! args_list)
      octave_throw_bad_alloc ();

    for (int i = 0; i < args.length (); ++i)
      {
        python_object obj = py_implicitly_convert_argument (args(i));

        if (pytave::is_py_kwargs_argument (obj))
          kwargs = pytave::update_py_dict (kwargs, obj);
        else
          PyList_Append (args_list, obj.release ());
      }

    python_object args_tuple = PyList_AsTuple (args_list);

    python_object retval = py_call_function (callable, args_tuple, kwargs);

    return retval.release ();
  }

  PyObject *
  py_call_function (PyObject *callable, PyObject *args, PyObject *kwargs)
  {
    python_object retval = PyEval_CallObjectWithKeywords (callable, args, kwargs);
    if (! retval)
      error_python_exception ();

    return retval.release ();
  }

  PyObject *
  py_run_string_safe (const std::string& expr, int start, PyObject *globals,
                      PyObject *locals)
  {
    bool alloc = false;

    if (! globals || (globals == Py_None))
      {
        python_object main = py_import_module ("__main__");
        globals = PyModule_GetDict (main);
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

    python_object retval = PyRun_StringFlags (expr.c_str (), start, globals, locals,
                                              &flags);

    if (alloc)
      Py_DECREF (globals);

    if (! retval)
      error_python_exception ();

    return retval.release ();
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
