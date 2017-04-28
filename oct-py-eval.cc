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
#include <octave/ov.h>
#include <octave/ovl.h>

#include "oct-py-eval.h"
#include "oct-py-util.h"
#include "octave_to_python.h"

namespace pytave
{

  PyObject *
  py_call_function (const std::string& func, const octave_value_list& args)
  {
    PyObject *func_obj = py_find_function (func);
    PyObject *retval = py_call_function (func_obj, args);
    Py_DECREF (func_obj);
    return retval;
  }

  PyObject *
  py_call_function (const std::string& func, PyObject *args, PyObject *kwargs)
  {
    PyObject *func_obj = py_find_function (func);
    PyObject *retval = py_call_function (func_obj, args, kwargs);
    Py_DECREF (func_obj);
    return retval;
  }

  PyObject *
  py_call_function (PyObject *callable, const octave_value_list& args)
  {
    PyObject *kwargs = nullptr;
    PyObject *args_list = PyList_New (0);
    if (! args_list)
      octave_throw_bad_alloc ();

    for (int i = 0; i < args.length (); ++i)
      {
        boost::python::object arg;
        pytave::octvalue_to_pyobj (arg, args(i));
        PyObject *obj = arg.ptr ();

        if (pytave::is_py_kwargs_argument (obj))
          kwargs = pytave::update_py_dict (kwargs, obj);
        else
          {
            Py_INCREF (obj);
            PyList_Append (args_list, obj);
          }
      }

    PyObject *args_tuple = PyList_AsTuple (args_list);
    Py_DECREF (args_list);

    PyObject *retval =  py_call_function (callable, args_tuple, kwargs);
    Py_DECREF (args_tuple);
    Py_XDECREF (kwargs);

    return retval;
  }

  PyObject *
  py_call_function (PyObject *callable, PyObject *args, PyObject *kwargs)
  {
    PyObject *retval = PyEval_CallObjectWithKeywords (callable, args, kwargs);
    if (! retval)
      throw boost::python::error_already_set ();

    return retval;
  }

  PyObject *
  py_run_string_safe (const std::string& expr, int start, PyObject *globals,
                      PyObject *locals)
  {
    bool alloc = false;

    if (! globals || (globals == Py_None))
      {
        PyObject *main = py_import_module ("__main__");
        globals = PyModule_GetDict (main);
        Py_DECREF (main);
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
