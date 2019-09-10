/*

SPDX-License-Identifier: GPL-3.0-or-later

Copyright (C) 2016-2019 Mike Miller
Copyright (C) 2019 Colin B. Macdonald

This file is part of Octave Pythonic.

Octave Pythonic is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Octave Pythonic is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Octave Pythonic; see the file COPYING.  If not, see
<https://www.gnu.org/licenses/>.

*/

#if defined (HAVE_CONFIG_H)
#  include <config.h>
#endif

#include <Python.h>
#include <octave/oct.h>

#include "oct-py-init.h"
#include "oct-py-object.h"
#include "oct-py-types.h"
#include "oct-py-util.h"

// PKG_ADD: autoload ("__py_class_name__", "__py_struct_from_dict__.oct");
// PKG_DEL: autoload ("__py_class_name__", which ("__py_struct_from_dict__.oct"), "remove");
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

  if (! (args(0).isobject () && args(0).class_name () == "pyobject"))
    error ("__py_class_name__: argument must be a valid Python object");

  pythonic::py_init ();

  pythonic::python_object obj = pythonic::pyobject_unwrap_object (args(0));
  std::string name = pythonic::py_object_class_name (obj);

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

// PKG_ADD: autoload ("__py_int64_scalar_value__", "__py_struct_from_dict__.oct");
// PKG_DEL: autoload ("__py_int64_scalar_value__", which ("__py_struct_from_dict__.oct"), "remove");
DEFUN_DLD (__py_int64_scalar_value__, args, ,
           "-*- texinfo -*-\n\
@deftypefn {} {} __py_int64_scalar_value__ (@var{x})\n\
Extract a scalar int64 value from the Python integer @var{x}.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  if (args.length () != 1)
    print_usage ();

  if (! (args(0).isobject () && args(0).class_name () == "pyobject"))
    error ("pyobject.int64: argument must be a Python object");

  pythonic::py_init ();

  pythonic::python_object obj = pythonic::pyobject_unwrap_object (args(0));
  octave_int64 retval = pythonic::extract_py_int64 (obj);

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

// PKG_ADD: autoload ("__py_uint64_scalar_value__", "__py_struct_from_dict__.oct");
// PKG_DEL: autoload ("__py_uint64_scalar_value__", which ("__py_struct_from_dict__.oct"), "remove");
DEFUN_DLD (__py_uint64_scalar_value__, args, ,
           "-*- texinfo -*-\n\
@deftypefn {} {} __py_uint64_scalar_value__ (@var{x})\n\
Extract a scalar uint64 value from the Python integer @var{x}.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  if (args.length () != 1)
    print_usage ();

  if (! (args(0).isobject () && args(0).class_name () == "pyobject"))
    error ("pyobject.uint64: argument must be a Python object");

  pythonic::py_init ();

  pythonic::python_object obj = pythonic::pyobject_unwrap_object (args(0));
  octave_uint64 retval = pythonic::extract_py_uint64 (obj);

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

// PKG_ADD: autoload ("__py_is_none__", "__py_struct_from_dict__.oct");
// PKG_DEL: autoload ("__py_is_none__", which ("__py_struct_from_dict__.oct"), "remove");
DEFUN_DLD (__py_is_none__, args, ,
           "-*- texinfo -*-\n\
@deftypefn  {} {} __py_is_none__ (@var{x})\n\
Check whether the Python object @var{obj} is the @code{None} object.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  if (args.length () != 1)
    print_usage ();

  pythonic::py_init ();

  pythonic::python_object obj = pythonic::pyobject_unwrap_object (args(0));

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

// PKG_ADD: autoload ("__py_isinstance__", "__py_struct_from_dict__.oct");
// PKG_DEL: autoload ("__py_isinstance__", which ("__py_struct_from_dict__.oct"), "remove");
DEFUN_DLD (__py_isinstance__, args, ,
           "-*- texinfo -*-\n\
@deftypefn  {} {} __py_isinstance__ (@var{x}, @var{type})\n\
Check whether @var{x} is an instance of a Python type named by the string\n\
@var{type}.\n\
\n\
For example\n\
\n\
@example\n\
@group\n\
__py_isinstance__ (py.list, \"py.list\")\n\
@result{} 1\n\
__py_isinstance__ (py.sys.version, \"py.str\")\n\
@result{} 1\n\
__py_isinstance__ (py.list, \"py.no.such.object\")\n\
@result{} 0\n\
@end group\n\
@end example\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();
  if (nargin != 2)
    print_usage ();

  if (! (args(0).isobject () && args(0).class_name () == "pyobject"))
    error ("pyobject.isa: X must be a Python object");

  if (! args(1).is_string ())
    error ("pyobject.isa: TYPE must be a string naming a Python type (py.*)");

  std::string typestr = args(1).string_value ();
  if (! ((typestr.size () > 3) && (typestr.compare (0, 3, "py.") == 0)))
    error ("pyobject.isa: TYPE must be a string naming a Python type (py.*)");

  typestr = typestr.substr (3);

  pythonic::py_init ();

  pythonic::python_object obj = pythonic::pyobject_unwrap_object (args(0));
  retval(0) = pythonic::py_isinstance (obj, typestr);

  return retval;
}

/*
%!assert (__py_isinstance__ (py.dict (), "py.dict"))
%!assert (__py_isinstance__ (py.list (), "py.list"))
%!assert (__py_isinstance__ (py.set (), "py.set"))
%!assert (__py_isinstance__ (py.str (), "py.str"))
%!assert (__py_isinstance__ (py.sys.version, "py.str"))
%!assert (__py_isinstance__ (py.sys.version, "py.object"))
%!assert (__py_isinstance__ (py.type (2), "py.type"))
%!assert (! __py_isinstance__ (py.dict (), "py.str"))
%!assert (! __py_isinstance__ (py.dict (), "py.list"))
%!assert (! __py_isinstance__ (py.dict (), "py.no.such.object"))
%!assert (! __py_isinstance__ (py.list (), "py.dict"))
%!assert (! __py_isinstance__ (py.list (), "py.no.such.object"))

%!error __py_isinstance__ ()
%!error __py_isinstance__ (pyeval ("None"))
%!error <must be a Python object> __py_isinstance__ (1, "py.float")
%!error <must be a string> __py_isinstance__ (pyeval ("None"), 2)
%!error <must be a string> __py_isinstance__ (pyeval ("None"), "object")
*/

// PKG_ADD: autoload ("__py_objstore_del__", "__py_struct_from_dict__.oct");
// PKG_DEL: autoload ("__py_objstore_del__", which ("__py_struct_from_dict__.oct"), "remove");
DEFUN_DLD (__py_objstore_del__, args, ,
           "-*- texinfo -*-\n\
@deftypefn {} {} __py_objstore_del__ (@var{key})\n\
Delete the Python object stored under @var{key} from the object store.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  if (args.length () != 1)
    print_usage ();

  pythonic::py_init ();

  uint64_t key = args(0).xuint64_scalar_value ("__py_objstore_del__: KEY must be an integer");
  pythonic::py_objstore_del (key);

  return ovl ();
}

// PKG_ADD: autoload ("__py_objstore_get__", "__py_struct_from_dict__.oct");
// PKG_DEL: autoload ("__py_objstore_get__", which ("__py_struct_from_dict__.oct"), "remove");
DEFUN_DLD (__py_objstore_get__, args, ,
           "-*- texinfo -*-\n\
@deftypefn {} {} __py_objstore_get__ (@var{key})\n\
Get the Python object stored under @var{key} from the object store.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  if (args.length () != 1)
    print_usage ();

  pythonic::py_init ();

  uint64_t key = args(0).xuint64_scalar_value ("__py_objstore_get__: KEY must be an integer");
  PyObject *obj = pythonic::py_objstore_get (key);

  if (! obj)
    error ("__py_objstore_get__: no existing Python object found for key %ju", key);

  octave_value retval = pythonic::pyobject_wrap_object (obj);

  return ovl (retval);
}

// PKG_ADD: autoload ("__py_objstore_put__", "__py_struct_from_dict__.oct");
// PKG_DEL: autoload ("__py_objstore_put__", which ("__py_struct_from_dict__.oct"), "remove");
DEFUN_DLD (__py_objstore_put__, args, ,
           "-*- texinfo -*-\n\
@deftypefn {} {} __py_objstore_put__ (@var{value})\n\
Convert @var{value} to a Python value and store in the object store.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  if (args.length () != 1)
    print_usage ();

  pythonic::py_init ();

  pythonic::python_object obj = pythonic::py_implicitly_convert_argument (args(0));

  if (! obj)
    error ("__py_objstore_put__: VALUE must be convertible to a Python value");

  uint64_t key = pythonic::py_objstore_put (obj.release ());

  return ovl (octave_uint64 (key));
}

// PKG_ADD: autoload ("__py_objstore_put_none__", "__py_struct_from_dict__.oct");
// PKG_DEL: autoload ("__py_objstore_put_none__", which ("__py_struct_from_dict__.oct"), "remove");
DEFUN_DLD (__py_objstore_put_none__, , ,
           "-*- texinfo -*-\n\
@deftypefn {} {} __py_objstore_put_none__ ()\n\
Store None in the object store and return its index.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  pythonic::py_init ();

  uint64_t key = pythonic::py_objstore_put (Py_None);

  return ovl (octave_uint64 (key));
}

// PKG_ADD: autoload ("__py_objstore_list__", "__py_struct_from_dict__.oct");
// PKG_DEL: autoload ("__py_objstore_list__", which ("__py_struct_from_dict__.oct"), "remove");
DEFUN_DLD (__py_objstore_list__, , ,
           "-*- texinfo -*-\n\
@deftypefn {} {} __py_objstore_list__ ()\n\
Show all objects and their ref counts in the object store.\n\
\n\
This is a private internal function not intended for direct use.\n\
\n\
TODO: Return a cell-array.  Do printing here\n\
@end deftypefn")
{
  pythonic::py_init ();

  pythonic::py_objstore_list ();

  return ovl ();
}

// PKG_ADD: autoload ("__py_string_value__", "__py_struct_from_dict__.oct");
// PKG_DEL: autoload ("__py_string_value__", which ("__py_struct_from_dict__.oct"), "remove");
DEFUN_DLD (__py_string_value__, args, ,
           "-*- texinfo -*-\n\
@deftypefn {} {} __py_string_value__ (@var{obj})\n\
Return the string value or representation of the Python object @var{obj}.\n\
\n\
This is a private internal function not intended for direct use.\n\
@end deftypefn")
{
  if (args.length () != 1)
    print_usage ();

  if (! (args(0).isobject () && args(0).class_name () == "pyobject"))
    error ("pyobject.char: argument must be a valid Python object");

  pythonic::py_init ();

  pythonic::python_object obj = pythonic::pyobject_unwrap_object (args(0));
  if (! obj)
    error ("pyobject.char: argument must be a valid Python object");

  std::string str;

  if (PyBytes_Check (obj) || PyUnicode_Check (obj))
    str = pythonic::extract_py_str (obj);
  else if (Py_TYPE (obj)->tp_str != nullptr)
    {
      pythonic::python_object s = PyObject_Str (obj);
      str = pythonic::extract_py_str (s);
    }
  else
    error ("pyobject.char: cannot convert Python object to string");

  return ovl (str);
}

/*
%!assert (__py_string_value__ (pyeval ("bytes()")), "")
%!assert (__py_string_value__ (pyeval ("str()")), "")
%!assert (__py_string_value__ (pyeval ("''")), "")
%!assert (__py_string_value__ (pyeval ("'Octave'")), "Octave")
%!assert (__py_string_value__ (pyeval ("b'Octave'")), "Octave")
%!assert (__py_string_value__ (pyeval ("r'Octave'")), "Octave")
%!assert (__py_string_value__ (pyeval ("u'Octave'")), "Octave")
%!assert (__py_string_value__ (pyeval ("[]")), "[]")
%!assert (__py_string_value__ (pyeval ("{}")), "{}")
%!assert (__py_string_value__ (pyeval ("dict()")), "{}")
%!assert (__py_string_value__ (pyeval ("list()")), "[]")
%!assert (__py_string_value__ (pyeval ("dir")), "<built-in function dir>")

%!error __py_string_value__ ()
%!error __py_string_value__ (pyeval ("''"), 2)
%!error <must be a valid Python object> __py_string_value__ ("Octave")
*/

DEFUN_DLD (__py_struct_from_dict__, args, ,
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
    print_usage ();

  if (! (args(0).isobject () && args(0).class_name () == "pyobject"))
    error ("pyobject.struct: argument must be a Python object");

  pythonic::py_init ();

  pythonic::python_object obj = pythonic::pyobject_unwrap_object (args(0));
  retval(0) = pythonic::extract_py_scalar_map (obj);

  return retval;
}

/*
%!assert (__py_struct_from_dict__ (pyeval ("{}")), struct ())
%!assert (__py_struct_from_dict__ (pyeval ("{'a': 1.0}")), struct ("a", 1))

%!error __py_struct_from_dict__ ()
%!error __py_struct_from_dict__ (pyeval ("{}"), 2)
%!error <must be a Python object> __py_struct_from_dict__ ("Octave")
%!error <unable to convert to an Octave struct> __py_struct_from_dict__ (pyeval ("[]"))
*/
