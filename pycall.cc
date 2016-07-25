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
s = pyeval (\"set([1, 2])\")\n\
pycall (s.add, 3)\n\
\n
r = pycall (s.add, 4)\n\
  @result{} v = [pyobject ...]\n\
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
  object res;
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

      std::vector<object> pyargs;
      for (int i = 1; i < nargin; i++)
        {
          object arg;
          pytave::octvalue_to_pyobj (arg, args(i));
          pyargs.push_back (arg);
        }

      switch (nargin - 1)
        {
        case 0:
          res = callable ();
          break;
        case 1:
          res = callable (pyargs[0]);
          break;
        case 2:
          res = callable (pyargs[0], pyargs[1]);
          break;
        case 3:
          res = callable (pyargs[0], pyargs[1], pyargs[2]);
          break;
        case 4:
          res = callable (pyargs[0], pyargs[1], pyargs[2], pyargs[3]);
          break;
        case 5:
          res = callable (pyargs[0], pyargs[1], pyargs[2], pyargs[3],
                          pyargs[4]);
          break;
        case 6:
          res = callable (pyargs[0], pyargs[1], pyargs[2], pyargs[3],
                          pyargs[4], pyargs[5]);
          break;
        case 7:
          res = callable (pyargs[0], pyargs[1], pyargs[2], pyargs[3],
                          pyargs[4], pyargs[5], pyargs[6]);
          break;
        case 8:
          res = callable (pyargs[0], pyargs[1], pyargs[2], pyargs[3],
                          pyargs[4], pyargs[5], pyargs[6], pyargs[7]);
          break;
        case 9:
          res = callable (pyargs[0], pyargs[1], pyargs[2], pyargs[3],
                          pyargs[4], pyargs[5], pyargs[6], pyargs[7],
                          pyargs[8]);
          break;
        case 10:
          res = callable (pyargs[0], pyargs[1], pyargs[2], pyargs[3],
                          pyargs[4], pyargs[5], pyargs[6], pyargs[7],
                          pyargs[8], pyargs[9]);
          break;
        default:
          error ("pycall: more than 10 arguments are not yet supported");
          break;
        }

      object hex_function = builtins_module.attr ("hex");
      object id_function = builtins_module.attr ("id");
      object idtmp = hex_function (id_function (res));
      id = extract<std::string> (idtmp);

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
      // Ensure we have a __InOct__ dict, and then put `res` into it
      exec ("if not (\"__InOct__\" in vars() or \"__InOct__\" in globals()):\n"
            "    __InOct__ = dict()\n"
            "    # FIXME: make it accessible elsewhere?\n"
            "    import __main__\n"
            "    __main__.__InOct__ = __InOct__\n",
            main_namespace, main_namespace);
      main_namespace["__InOct__"][id] = res;
      // Create @pyobject
      retval = feval ("pyobject", ovl (id), 1);
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

## Test round trip type preservation / conversion
%!test
%! pyexec ("def roundtrip(x): return x");
%! values = { 0, pi, 2j, eps, false, true, version, "Hello world", ...
%!            [1, 2, 3], eye (4) };
%! for i = 1:numel (values)
%!   assert (pycall ("roundtrip", values{i}), values{i});
%! endfor

%!error <argument type conversion>
%! pyexec ("def intwrapper(x):\n    return int(x)\n");
%! pycall ("intwrapper", ftp ());

%!test
%! pyexec ("def pyfunc(x):\n    return 2*x");
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
%! pyexec ("def raiseException ():\n  raise NameError ('oops')")
%! pycall ("raiseException")

## None as a return value
%!test
%! f = pyeval ("lambda: None");
%! r = pycall (f);
%! isNone = pyeval("lambda a: a is None");
%! assert (isNone (r))

## But returning None will not set "ans"
%!test
%! f = pyeval ("lambda: None");
%! clear ans
%! pycall (f);
%! assert (! exist ("ans", "var"))
*/
