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

#include "exceptions.h"
#include "oct-py-init.h"
#include "oct-py-object.h"
#include "oct-py-types.h"
#include "oct-py-util.h"
#include "octave_to_python.h"

DEFUN_DLD (__py_class_name__, args, ,
           "-*- texinfo -*-\n\
@deftypefn  {} {} __py_class_name__ (@var{obj})\n\
Return the name of the class of the Python object @var{obj}.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  if (args.length () != 1)
    print_usage ();

  if (! (args(0).is_object () && args(0).class_name () == "pyobject"))
    error ("__py_class_name__: argument must be a valid Python object");

  pytave::py_init ();

  pytave::python_object obj = pytave::pyobject_unwrap_object (args(0));
  std::string name = pytave::py_object_class_name (obj);

  return ovl (name);
}

/*
%!assert (__py_class_name__ (pyeval ("None")), "NoneType")
%!assert (__py_class_name__ (pyeval ("'Octave'")), "str")
%!assert (__py_class_name__ (pyeval ("{}")), "dict")
%!assert (__py_class_name__ (pyeval ("[]")), "list")
%!assert (__py_class_name__ (pyeval ("()")), "tuple")
%!assert (__py_class_name__ (pyeval ("__import__('array').array('d')")), "array.array")

%% Test an anonymous class with its __module__ property set to None
%!assert (__py_class_name__ (pyeval ("[[t() for t.__module__ in (None,)][0] for t in (type('foo', (), {}),)][0]")), "foo")

%!error __py_class_name__ ()
%!error __py_class_name__ (1)
%!error __py_class_name__ (1, 2)
*/

DEFUN_DLD (__py_int64_scalar_value__, args, nargout,
           "-*- texinfo -*-\n\
@deftypefn {} {} __py_int64_scalar_value__ (@var{x})\n\
Extract a scalar int64 value from the Python integer @var{x}.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  if (args.length () != 1)
    print_usage ();

  if (! (args(0).is_object () && args(0).class_name () == "pyobject"))
    error ("pyobject.int64: argument must be a Python object");

  pytave::py_init ();

  pytave::python_object obj = pytave::pyobject_unwrap_object (args(0));
  if (! obj)
    error ("pyobject.int64: argument must be a valid Python object");

  octave_int64 retval;

  try
    {
      retval = pytave::extract_py_int64 (obj);
    }
  catch (pytave::object_convert_exception const &)
    {
      error ("pyobject.int64: argument must be a Python int or long object");
    }
  catch (boost::python::error_already_set const &)
    {
      std::string message = pytave::fetch_exception_message ();
      error ("pyobject.int64: %s", message.c_str ());
    }

  return ovl (retval);
}

/*
%!assert (__py_int64_scalar_value__ (pyobject (pyeval ("0"))), int64 (0))
%!assert (__py_int64_scalar_value__ (pyobject (pyeval ("2**62"))), int64 (2^62))
%!assert (__py_int64_scalar_value__ (pyobject (pyeval ("-2**62"))), int64 (-2^62))
%!assert (__py_int64_scalar_value__ (pyobject (pyeval ("2**128"))), intmax ("int64"))
%!assert (__py_int64_scalar_value__ (pyobject (pyeval ("-2**128"))), intmin ("int64"))

%!error __py_int64_scalar_value__ ()
%!error __py_int64_scalar_value__ (1)
%!error __py_int64_scalar_value__ (pyeval ("None"))
%!error __py_int64_scalar_value__ (1, 2)
*/

DEFUN_DLD (__py_uint64_scalar_value__, args, nargout,
           "-*- texinfo -*-\n\
@deftypefn {} {} __py_uint64_scalar_value__ (@var{x})\n\
Extract a scalar uint64 value from the Python integer @var{x}.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  if (args.length () != 1)
    print_usage ();

  if (! (args(0).is_object () && args(0).class_name () == "pyobject"))
    error ("pyobject.uint64: argument must be a Python object");

  pytave::py_init ();

  pytave::python_object obj = pytave::pyobject_unwrap_object (args(0));
  if (! obj)
    error ("pyobject.uint64: argument must be a valid Python object");

  octave_uint64 retval;

  try
    {
      retval = pytave::extract_py_uint64 (obj);
    }
  catch (pytave::object_convert_exception const &)
    {
      error ("pyobject.uint64: argument must be a Python int or long object");
    }
  catch (boost::python::error_already_set const &)
    {
      std::string message = pytave::fetch_exception_message ();
      error ("pyobject.uint64: %s", message.c_str ());
    }

  return ovl (retval);
}

/*
%!assert (__py_uint64_scalar_value__ (pyobject (pyeval ("0"))), uint64 (0))
%!assert (__py_uint64_scalar_value__ (pyobject (pyeval ("2**62"))), uint64 (2^62))
%!assert (__py_uint64_scalar_value__ (pyobject (pyeval ("2**128"))), intmax ("uint64"))
%!assert (__py_uint64_scalar_value__ (pyobject (pyeval ("-2**128"))), intmin ("uint64"))

%!error __py_uint64_scalar_value__ ()
%!error __py_uint64_scalar_value__ (1)
%!error __py_uint64_scalar_value__ (pyeval ("None"))
%!error __py_uint64_scalar_value__ (1, 2)
*/

DEFUN_DLD (__py_is_none__, args, nargout,
           "-*- texinfo -*-\n\
@deftypefn  {} {} __py_is_none__ (@var{x})\n\
Check whether the Python object @var{obj} is the @code{None} object.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  if (args.length () != 1)
    print_usage ();

  pytave::py_init ();

  pytave::python_object obj = pytave::pyobject_unwrap_object (args(0));

  return ovl (obj.is_none ());
}

/*
%!assert (__py_is_none__ (pyobject ()))
%!assert (__py_is_none__ (pyeval ("None")))
%!assert (! __py_is_none__ (1))
%!assert (! __py_is_none__ ("None"))
%!assert (! __py_is_none__ (pyobject (1)))
%!assert (! __py_is_none__ (pyobject ("None")))

%!error __py_is_none__ ()
%!error __py_is_none__ (1, 2)
*/

DEFUN_DLD (__py_isinstance__, args, nargout,
           "-*- texinfo -*-\n\
@deftypefn  {} {} __py_isinstance__ (@var{x})\n\
Check whether the Python object @var{obj} is an instance of a Python type\n\
specified by the string @var{type}.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();
  if (nargin != 2)
    {
      print_usage ();
      return retval;
    }

  if (! (args(0).is_object () && args(0).class_name () == "pyobject"))
    error ("pyobject.isa: OBJ must be a Python object");

  if (! args(1).is_string ())
    error ("pyobject.isa: TYPE must be a string naming a Python type (py.*)");

  std::string typestr = args(1).string_value ();
  if (! ((typestr.size () > 3) && (typestr.compare (0, 3, "py.") == 0)))
    error ("pyobject.isa: TYPE must be a string naming a Python type (py.*)");

  typestr = typestr.substr (3);

  pytave::py_init ();

  pytave::python_object obj = pytave::pyobject_unwrap_object (args(0));
  pytave::python_object type = pytave::py_find_type (typestr);
  retval(0) = pytave::py_isinstance (obj, type);

  return retval;
}

DEFUN_DLD (__py_objstore_del__, args, nargout,
           "-*- texinfo -*-\n\
@deftypefn {} {} __py_objstore_del__ (@var{key})\n\
Delete the Python object stored under @var{key} from the object store.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  if (args.length () != 1)
    print_usage ();

  pytave::py_init ();

  uint64_t key = args(0).xuint64_scalar_value ("__py_objstore_del__: KEY must be an integer");
  pytave::py_objstore_del (key);

  return ovl ();
}

DEFUN_DLD (__py_objstore_get__, args, nargout,
           "-*- texinfo -*-\n\
@deftypefn {} {} __py_objstore_get__ (@var{key})\n\
Get the Python object stored under @var{key} from the object store.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  if (args.length () != 1)
    print_usage ();

  pytave::py_init ();

  uint64_t key = args(0).xuint64_scalar_value ("__py_objstore_get__: KEY must be an integer");
  PyObject *obj = pytave::py_objstore_get (key);

  if (! obj)
    error ("__py_objstore_get__: no existing Python object found for key %ju", key);

  octave_value retval = pytave::pyobject_wrap_object (obj);

  return ovl (retval);
}

DEFUN_DLD (__py_objstore_put__, args, nargout,
           "-*- texinfo -*-\n\
@deftypefn {} {} __py_objstore_put__ (@var{value})\n\
Convert @var{value} to a Python value and store in the object store.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  if (args.length () != 1)
    print_usage ();

  pytave::py_init ();

  // FIXME: PyObject *obj = convert argument to Python (args(0));
  PyObject *obj = nullptr;
  try
    {
      boost::python::object arg;
      pytave::octvalue_to_pyobj (arg, args(0));
      obj = arg.ptr ();
      Py_INCREF (obj);
    }
  catch (pytave::value_convert_exception const &)
    {
    }

  if (! obj)
    error ("__py_objstore_put__: VALUE must be convertible to a Python value");

  uint64_t key = pytave::py_objstore_put (obj);
  Py_DECREF (obj);

  return ovl (octave_uint64 (key));
}

DEFUN_DLD (__py_string_value__, args, nargout,
           "-*- texinfo -*-\n\
@deftypefn {} {} __py_string_value__ (@var{obj})\n\
Return the string value or representation of the Python object @var{obj}.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  if (args.length () != 1)
    print_usage ();

  if (! (args(0).is_object () && args(0).class_name () == "pyobject"))
    error ("pyobject.char: argument must be a valid Python object");

  pytave::py_init ();

  pytave::python_object obj = pytave::pyobject_unwrap_object (args(0));
  if (! obj)
    error ("pyobject.char: argument must be a valid Python object");

  std::string str;

  if (PyBytes_Check (obj) || PyUnicode_Check (obj))
    str = pytave::extract_py_str (obj);
  else if (Py_TYPE (obj)->tp_str != nullptr)
    {
      pytave::python_object s = PyObject_Str (obj);
      str = pytave::extract_py_str (s);
    }
  else
    error ("pyobject.char: cannot convert Python object to string");

  return ovl (str);
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

  pytave::py_init ();

  pytave::python_object obj = pytave::pyobject_unwrap_object (args(0));
  retval(0) = pytave::extract_py_scalar_map (obj);

  return retval;
}

/*
## No test needed for internal helper function.
%!assert (1)
*/
