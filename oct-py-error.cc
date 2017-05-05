/*

Copyright (C) 2017 Mike Miller

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
#include <octave/error.h>

#include "oct-py-error.h"
#include "oct-py-eval.h"
#include "oct-py-object.h"
#include "oct-py-types.h"

namespace pytave
{

  void
  error_conversion_invalid_python_object (const std::string& to)
  {
    error ("unable to convert to %s, invalid Python object", to.c_str ());
  }

  void
  error_conversion_mismatch_python_type (const std::string& to,
                                         const std::string& must)
  {
    error ("unable to convert to %s, must be a Python %s", to.c_str (),
           must.c_str ());
  }

  void
  error_python_exception ()
  {
    const char *format_exception_only = "traceback.format_exception_only";

    PyObject *ptype, *pvalue, *ptraceback;
    PyErr_Fetch (&ptype, &pvalue, &ptraceback);
    PyErr_NormalizeException (&ptype, &pvalue, &ptraceback);

    python_object args = PyTuple_Pack (2, ptype, pvalue);
    python_object lines = py_call_function (format_exception_only, args);

    if (lines && PySequence_Check (lines))
      {
        Py_ssize_t len = PySequence_Size (lines);
        python_object last_line = PySequence_GetItem (lines, len - 1);

        std::string msg = extract_py_str (last_line);
        if (msg.back () == '\n')
          msg.resize (msg.size () - 1);

        error ("%s", msg.c_str ());
      }
    else
      {
        PyErr_Restore (ptype, pvalue, ptraceback);
        PyErr_Print ();
        error ("runtime failed to get exception information from %s",
               format_exception_only);
      }
  }

}
