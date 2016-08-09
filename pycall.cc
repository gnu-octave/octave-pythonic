/*

Copyright (C) 2015-2016 Mike Miller

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

#include <boost/python.hpp>
#include <boost/python/numeric.hpp>

#include <octave/oct.h>
#include <octave/parse.h>

#define PYTAVE_DO_DECLARE_SYMBOL
#include "arrayobjectdefs.h"
#include "exceptions.h"
#include "octave_to_python.h"
#include "python_to_octave.h"

using namespace boost::python;

DEFUN_DLD (pycall, args, nargout,
           "-*- texinfo -*-\n\
@deftypefn  {} {} pycall (@var{func})\n\
@deftypefnx {} {@var{x} =} pycall (@var{func})\n\
@deftypefnx {} {@var{x} =} pycall (@var{func}, @var{arg1}, @var{arg2}, @dots{})\n\
Call a Python function or callable, passing Octave values as arguments.\n\
\n\
Examples:\n\
@example\n\
@group\n\
pycall (\"int\", 6)\n\
  @result{} 6\n\
pycall (\"os.getuid\")\n\
  @result{} ...\n\
pycall (\"math.sqrt\", 2)\n\
  @result{} 1.4142\n\
@end group\n\
@end example\n\
\n\
If the callable has no return, and an lvalue is specified, it will be set\n\
to @code{None}.  However, if no lvalue was specified, @code{ans} will not\n\
be set.  For example:\n\
@example\n\
@group\n\
s = pyeval (\"set([1, 2])\");\n\
pycall (s.add, 3)\n\
\n\
r = pycall (s.add, 4)\n\
  @result{} r = [pyobject ...]\n\
\n\
      None\n\
\n\
@end group\n\
@end example\n\
\n\
@seealso{pyeval, pyexec}\n\
@end deftypefn")
{
  octave_value_list retval;
  std::string id;

  int nargin = args.length ();

  if (nargin < 1)
    {
      print_usage ();
      return retval;
    }

  bool func_by_name = false;

  if (args(0).is_string ())
    func_by_name = true;
  else if (args(0).is_object () && args(0).class_name () == "pyobject")
    func_by_name = false;
  else
    error ("pycall: FUNC must be a string or a Python reference");

  Py_Initialize ();

  pytave::init_exceptions ();
  numeric::array::set_module_and_type ("numpy", "ndarray");
  _import_array ();

  object main_module = import ("__main__");
  object main_namespace = main_module.attr ("__dict__");
#if PY_VERSION_HEX >= 0x03000000
  object builtins_module = import ("builtins");
#else
  object builtins_module = import ("__builtin__");
#endif

  try
    {
      object callable;

      if (func_by_name)
        {
          std::string module;
          std::string func = args(0).string_value ();

          size_t idx = func.rfind (".");
          if (idx != std::string::npos)
            {
              module = func.substr (0, idx);
              func = func.substr (idx + 1);
            }

          object mod;
          if (module.empty ())
            {
              if (PyObject_HasAttrString (main_module.ptr (), func.c_str ()))
                mod = main_module;
              else
                mod = builtins_module;
            }
          else
            mod = import (module.c_str ());

          callable = mod.attr (func.c_str ());
        }
      else
        {
          octave_value_list tmp = feval ("getid", ovl (args(0)), 1);
          std::string hexid = tmp(0).string_value ();
          callable = main_module.attr ("__InOct__")[hexid];
        }

      PyObject *pyargs = PyTuple_New (nargin - 1);
      for (int i = 1; i < nargin; i++)
        {
          object arg;
          pytave::octvalue_to_pyobj (arg, args(i));
          PyObject *obj = arg.ptr ();
          Py_INCREF (obj);
          PyTuple_SET_ITEM (pyargs, i - 1, obj);
        }

      PyObject *result = PyEval_CallObjectWithKeywords (callable.ptr (), pyargs, 0);
      object res = object (handle<PyObject> (result));

      // Ensure reasonable "ans" behaviour, consistent with Python's "_".
      if (nargout > 0 || ! res.is_none ())
        {
          octave_value val;
          pytave::pyobj_to_octvalue (val, res);
          retval(0) = val;
        }
    }
  catch (pytave::object_convert_exception const &)
    {
      error ("pyexec: error in return value type conversion");
    }
  catch (pytave::value_convert_exception const &)
    {
      error ("pycall: error in argument type conversion");
    }
  catch (error_already_set const &)
    {
      std::string message = pytave::fetch_exception_message ();
      error ("pycall: %s", message.c_str ());
    }

  return retval;
}

/*
%!assert (ischar (pycall ("os.getcwd")))
%!assert (isreal (pycall ("random.random")))
%!assert (pycall ("math.exp", 3), exp (3))
%!assert (pycall ("math.trunc", pi), fix (pi))
%!assert (pycall ("math.sqrt", 2), sqrt (2))
%!assert (pycall ("cmath.sqrt", 2j), sqrt (2j))
%!assert (pycall ("int", 10.2), 10)
%!assert (isa (pycall ("object"), "pyobject"))
%!assert (isa (pycall ("dict"), "pyobject"))
%!assert (isa (pycall ("list"), "pyobject"))
%!assert (isa (pycall ("tuple"), "pyobject"))

## Test argument type conversion of values into Python
%!test
%! pyexec (["def typename(x):\n" ...
%!          "    s = type(x).__name__\n" ...
%!          "    if s == 'long':\n" ...
%!          "        return 'int'\n" ...
%!          "    return s"]);
%!assert (pycall ("typename", 0), "float")
%!assert (pycall ("typename", pi), "float")
%!assert (pycall ("typename", 2j), "complex")
%!assert (pycall ("typename", int32 (0)), "int")
%!assert (pycall ("typename", false), "bool")
%!assert (pycall ("typename", true), "bool")
%!assert (pycall ("typename", "Hello world"), "str")
%!assert (pycall ("typename", char ([1, 2, 3])), "str")

## Test construction of sequence types from cell arrays
%!assert (char (pycall ("list")), "[]")
%!assert (char (pycall ("list", {})), "[]")
%!assert (char (pycall ("list", {1, 2, 3})), "[1.0, 2.0, 3.0]")
%!assert (char (pycall ("list", {int8(1), int8(2), int8(3)})), "[1, 2, 3]")
%!assert (char (pycall ("tuple")), "()")
%!assert (char (pycall ("tuple", {})), "()")
%!assert (char (pycall ("tuple", {1, 2, 3})), "(1.0, 2.0, 3.0)")
%!assert (char (pycall ("tuple", {int8(1), int8(2), int8(3)})), "(1, 2, 3)")
%!error (pycall ("list", {1, 2, 3; 4, 5, 6}))
%!error (pycall ("dict", {1, 2, 3}))

## Test round trip type preservation / conversion
%!test
%! pyexec ("def roundtrip(x): return x");
%! values = { 0, pi, 2j, eps, false, true, version, "Hello world", ...
%!            [1, 2, 3], eye (4) };
%! for i = 1:numel (values)
%!   assert (pycall ("roundtrip", values{i}), values{i});
%! endfor

## Test conversion of integer types into Python
%!assert (pycall (pyeval ("lambda x: type(x) == type(0) and x ==        0"), int8 (0)))
%!assert (pycall (pyeval ("lambda x: type(x) == type(0) and x == -2**7   "), intmin ("int8")))
%!assert (pycall (pyeval ("lambda x: type(x) == type(0) and x ==  2**7 -1"), intmax ("int8")))
%!assert (pycall (pyeval ("lambda x: type(x) == type(0) and x ==        0"), intmin ("uint8")))
%!assert (pycall (pyeval ("lambda x: type(x) == type(0) and x ==  2**8 -1"), intmax ("uint8")))
%!assert (pycall (pyeval ("lambda x: type(x) == type(0) and x ==        0"), int16 (0)))
%!assert (pycall (pyeval ("lambda x: type(x) == type(0) and x == -2**15  "), intmin ("int16")))
%!assert (pycall (pyeval ("lambda x: type(x) == type(0) and x ==  2**15-1"), intmax ("int16")))
%!assert (pycall (pyeval ("lambda x: type(x) == type(0) and x ==        0"), intmin ("uint16")))
%!assert (pycall (pyeval ("lambda x: type(x) == type(0) and x ==  2**16-1"), intmax ("uint16")))
%!assert (pycall (pyeval ("lambda x: type(x) == type(0) and x ==        0"), int32 (0)))
%!assert (pycall (pyeval ("lambda x: type(x) == type(0) and x == -2**31  "), intmin ("int32")))
%!assert (pycall (pyeval ("lambda x: type(x) == type(0) and x ==  2**31-1"), intmax ("int32")))
%!assert (pycall (pyeval ("lambda x: type(x) == type(2**64) and x ==        0"), intmin ("uint32")))
%!assert (pycall (pyeval ("lambda x: type(x) == type(2**64) and x ==  2**32-1"), intmax ("uint32")))
%!assert (pycall (pyeval ("lambda x: type(x) == type(2**64) and x ==        0"), int64 (0)))
%!assert (pycall (pyeval ("lambda x: type(x) == type(2**64) and x == -2**63  "), intmin ("int64")))
%!assert (pycall (pyeval ("lambda x: type(x) == type(2**64) and x ==  2**63-1"), intmax ("int64")))
%!assert (pycall (pyeval ("lambda x: type(x) == type(2**64) and x ==        0"), intmin ("uint64")))
%!assert (pycall (pyeval ("lambda x: type(x) == type(2**64) and x ==  2**64-1"), intmax ("uint64")))

%!error <argument type conversion>
%! pyexec ("def intwrapper(x): return int(x)");
%! pycall ("intwrapper", ftp ());

%!test
%! pyexec ("def pyfunc(x): return 2*x");
%! z = pycall ("pyfunc", [20 20]);
%! assert (z, [40 40])

%!test
%! pyexec (["def pyfunc(x):\n" ...
%!         "    if x is True:\n        return 30\n" ...
%!         "    elif x is False:\n        return 20\n" ...
%!         "    else:\n        return 10"]);
%! assert (pycall ("pyfunc", true), 30)
%! assert (pycall ("pyfunc", false), 20)
%! assert (pycall ("pyfunc", 10), 10)

%!error <NameError>
%! pyexec ("def raiseException(): raise NameError('oops')")
%! pycall ("raiseException")

## None as a return value
%!test
%! f = pyeval ("lambda: None");
%! r = pycall (f);
%! is_none = pyeval ("lambda x: x is None");
%! assert (is_none (r))

## But returning None will not set "ans"
%!test
%! f = pyeval ("lambda: None");
%! clear ans
%! pycall (f);
%! assert (! exist ("ans", "var"))
*/
