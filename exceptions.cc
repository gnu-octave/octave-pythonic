/*

Copyright (C) 2015-2016 Mike Miller
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

#if defined (HAVE_CONFIG_H)
#  include <config.h>
#endif

#include "exceptions.h"
#include "oct-py-eval.h"
#include "oct-py-types.h"

namespace pytave
{

  PyObject *octave_error_exception::excclass = 0;
  PyObject *value_convert_exception::excclass = 0;
  PyObject *object_convert_exception::excclass = 0;
  PyObject *octave_parse_exception::excclass = 0;
  PyObject *variable_name_exception::excclass = 0;

  bool init_exceptions (void)
  {
    return (octave_error_exception::init ()
            && value_convert_exception::init ()
            && object_convert_exception::init ()
            && octave_parse_exception::init ()
            && variable_name_exception::init ());
  }

  std::string fetch_exception_message (void)
  {
    PyObject *ptype, *pvalue, *ptraceback;
    PyObject *formatted_list, *pargs;

    PyErr_Fetch (&ptype, &pvalue, &ptraceback);
    PyErr_NormalizeException (&ptype, &pvalue, &ptraceback);
    std::string message;

    pargs = PyTuple_New (2);
    PyTuple_SetItem (pargs, 0, ptype);
    PyTuple_SetItem (pargs, 1, pvalue);
    formatted_list = py_call_function \
      ("traceback.format_exception_only", pargs);
    Py_DECREF (pargs);

    if (formatted_list && PyList_Check (formatted_list))
      {
        int len = PyList_Size (formatted_list);

        for (int i = 0; i < len; i++)
          message.append (extract_py_str (PyList_GetItem (formatted_list, i)));
        Py_DECREF (formatted_list);
      }
    else
      {
        PyErr_Restore (ptype, pvalue, ptraceback);
        PyErr_Print ();
        message = std::string ("exceptions.cc (pytave::fetch_exception_message): Cannot call 'format_exceptions_only' for the traceback");
      }
    return message;
  }
}
