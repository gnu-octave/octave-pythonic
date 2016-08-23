/*

Copyright (C) 2016 Mike Miller

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

#include <Python.h>
#include <octave/oct.h>

#define PYTAVE_DO_DECLARE_SYMBOL
#include "arrayobjectdefs.h"
#include "exceptions.h"
#include "oct-py-types.h"
#include "octave_to_python.h"

DEFUN_DLD (__py_int64_scalar_value__, args, nargout,
           "-*- texinfo -*-\n\
@deftypefn  {} {} __py_int64_scalar_value__ (@var{x})\n\
Extract a scalar int64 value from the Python integer @var{x}.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();
  if (nargin != 1)
    {
      print_usage ();
      return retval;
    }

  if (! (args(0).is_object () && args(0).class_name () == "pyobject"))
    error ("pyobject.int64: argument must be a Python object");

  Py_Initialize ();

  try
    {
      // FIXME: PyObject *obj = look up stored pyobject reference (args(0));
      boost::python::object arg;
      pytave::octvalue_to_pyobj (arg, args(0));
      PyObject *obj = arg.ptr ();

      retval(0) = octave_int64 (pytave::extract_py_int64 (obj));
    }
  catch (pytave::object_convert_exception const &)
    {
      error ("pyobject.int64: error in return value type conversion");
    }
  catch (boost::python::error_already_set const &)
    {
      std::string message = pytave::fetch_exception_message ();
      error ("pyobject.int64: %s", message.c_str ());
    }

  return retval;
}

DEFUN_DLD (__py_struct_from_dict__, args, nargout,
           "-*- texinfo -*-\n\
@deftypefn  {} {} __py_struct_from_dict__ (@var{dict})\n\
Extract a scalar struct from the Python dict @var{dict}.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  octave_value_list retval;
  std::string id;

  int nargin = args.length ();

  if (nargin != 1)
    {
      print_usage ();
      return retval;
    }

  if (! (args(0).is_object () && args(0).class_name () == "pyobject"))
    error ("pyobject.struct: argument must be a Python object");

  Py_Initialize ();

  try
    {
      // FIXME: PyObject *obj = look up stored pyobject reference (args(0));
      boost::python::object arg;
      pytave::octvalue_to_pyobj (arg, args(0));
      PyObject *obj = arg.ptr ();

      retval(0) = pytave::extract_py_scalar_map (obj);
    }
  catch (pytave::object_convert_exception const &)
    {
      error ("pyobject.struct: error in return value type conversion");
    }
  catch (boost::python::error_already_set const &)
    {
      std::string message = pytave::fetch_exception_message ();
      error ("pyobject.struct: %s", message.c_str ());
    }

  return retval;
}

/*
## No test needed for internal helper function.
%!assert (1)
*/
