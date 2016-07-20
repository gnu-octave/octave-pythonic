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

#include <oct.h>

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
@seealso{pyeval, pyexec}\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin < 1)
    {
      print_usage ();
      return retval;
    }

  std::string module;
  std::string func = args(0).string_value ();

  size_t idx = func.rfind (".");
  if (idx != std::string::npos)
    {
      module = func.substr (0, idx);
      func = func.substr (idx + 1);
    }

  Py_Initialize ();

  pytave::init_exceptions ();
  numeric::array::set_module_and_type ("numpy", "ndarray");
  _import_array ();

  object main_module = import ("__main__");
#if PY_VERSION_HEX >= 0x03000000
  object builtins_module = import ("builtins");
#else
  object builtins_module = import ("__builtin__");
#endif

  try
    {
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

      object callable = mod.attr (func.c_str ());

      std::vector<object> pyargs;
      for (int i = 1; i < nargin; i++)
        {
          object arg;
          pytave::octvalue_to_pyobj (arg, args(i));
          pyargs.push_back (arg);
        }

      object res;

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

      if (! res.is_none ())
        {
          octave_value val;
          pytave::pyobj_to_octvalue (val, res);
          retval(0) = val;
        }
    }
  catch (pytave::object_convert_exception const &)
    {
      error ("pycall: error in return value type conversion");
    }
  catch (pytave::value_convert_exception const &)
    {
      error ("pycall: error in argument type conversion");
    }
  catch (error_already_set const &)
    {
      PyObject *ptype, *pvalue, *ptraceback;
      PyErr_Fetch (&ptype, &pvalue, &ptraceback);

      try
        {
          std::string message = extract<std::string> (pvalue);
          error ("pycall: %s", message.c_str ());
        }
      catch (error_already_set const &)
        {
          PyErr_Restore (ptype, pvalue, ptraceback);
          PyErr_Print ();
        }
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

## Test argument type conversion of values into Python
%!test
%! pyexec ("def typename(x): return type(x).__name__");
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
*/
