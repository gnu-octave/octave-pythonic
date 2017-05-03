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

#include <octave/oct.h>
#include <octave/parse.h>

#include "exceptions.h"
#include "oct-py-eval.h"
#include "oct-py-init.h"
#include "oct-py-object.h"
#include "oct-py-util.h"
#include "octave_to_python.h"
#include "python_to_octave.h"

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
pycall (\"float\", 6)\n\
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

  if (! (args(0).is_string () || (args(0).is_object ()
                                  && args(0).class_name () == "pyobject")))
    error ("pycall: FUNC must be a string or a Python reference");

  pytave::py_init ();

  try
    {
      pytave::python_object callable;
      if (args(0).is_string ())
        {
          callable = pytave::py_find_function (args(0).string_value ());
          if (! callable)
            error ("pycall: no such Python function or callable: %s",
                   args(0).string_value ().c_str ());
        }
      else
        {
          callable = pytave::pyobject_unwrap_object (args(0));
          if (! callable)
            error("pycall: FUNC must be a valid Python reference");
        }

      octave_value_list arglist = args.slice (1, nargin - 1);
      PyObject *result = pytave::py_call_function (callable, arglist);
      boost::python::object res { boost::python::handle<> (result) };

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
  catch (boost::python::error_already_set const &)
    {
      std::string message = pytave::fetch_exception_message ();
      error ("pycall: %s", message.c_str ());
    }

  return retval;
}

/*
%!assert (isreal (pycall ("random.random")))
%!assert (double (pycall ("math.exp", 3)), exp (3))
%!assert (double (pycall ("math.trunc", pi)), fix (pi))
%!assert (double (pycall ("math.sqrt", 2)), sqrt (2))
%!assert (double (pycall ("cmath.sqrt", 2j)), sqrt (2j))
%!assert (double (pycall ("int", 10.2)), 10)
%!assert (isa (pycall ("os.getcwd"), "pyobject"))
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
%!assert (char (pycall ("typename", 0)), "float")
%!assert (char (pycall ("typename", pi)), "float")
%!assert (char (pycall ("typename", 2j)), "complex")
%!assert (char (pycall ("typename", int32 (0))), "int")
%!assert (char (pycall ("typename", false)), "bool")
%!assert (char (pycall ("typename", true)), "bool")
%!assert (char (pycall ("typename", "Hello world")), "str")
%!assert (char (pycall ("typename", char ([1, 2, 3]))), "str")

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

## Test failure to convert char arrays to strings
%!error (pycall ("str", ("hello")'))
%!error (pycall ("str", ["hello"; "world"]))

## Test construction of dict from pyargs
%!test
%! a = pycall ("dict", pyargs ("a", 1, "b", 2, "c", 3));
%! assert (sort (cellfun (@char, cell (pycall ("list", a.keys ())), "uniformoutput", false)), {"a", "b", "c"})
%! assert (sort (double (pycall ("array.array", "d", a.values ()))), [1, 2, 3])

## Test copy construction of dict from dict
%!test
%! a = pycall ("dict", pyargs ("a", 1, "b", 2, "c", 3));
%! b = pycall ("dict", a);
%! assert (isequal (a, b))

## Test construction of dict from sequence of key value pairs
%!test
%! a = pycall ("dict", pyargs ("a", 1, "b", 2, "c", 3));
%! b = pycall ("dict", pycall ("zip", {"a", "b", "c"}, {1, 2, 3}));
%! assert (isequal (a, b))

## Test round trip type preservation / conversion
%!test
%! pyexec ("def roundtrip(x): return x");
%! values = { 0, pi, 2j, eps, false, true };
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

## Test conversion of integer types from Python
%!test
%! if (pyeval ("__import__('sys').hexversion >= 0x03000000"))
%!   assert (isa (pycall ("int",  0), "pyobject"))
%!   assert (isa (pycall ("int",  2^31-1), "pyobject"))
%!   assert (isa (pycall ("int", -2^31), "pyobject"))
%!   assert (double (pycall ("int",  0)), 0)
%!   assert (double (pycall ("int",  2^31-1)), 2^31-1)
%!   assert (double (pycall ("int", -2^31)), -2^31)
%! else
%!   assert (pycall ("int",  0), int64 (0))
%!   assert (pycall ("int",  2^31-1), int64 (2^31-1))
%!   assert (pycall ("int", -2^31), int64 (-2^31))
%!   assert (isa (pycall ("long",  0), "pyobject"))
%!   assert (isa (pycall ("long",  2^31-1), "pyobject"))
%!   assert (isa (pycall ("long", -2^31), "pyobject"))
%!   assert (double (pycall ("long",  0)), 0)
%!   assert (double (pycall ("long",  2^31-1)), 2^31-1)
%!   assert (double (pycall ("long", -2^31)), -2^31)
%! endif
%!assert (isa (pycall ("int", 2^100), "pyobject"))

%!test
%! pyexec (["def pyfunc(x):\n" ...
%!         "    if x is True:\n        return 30\n" ...
%!         "    elif x is False:\n        return 20\n" ...
%!         "    else:\n        return 10"]);
%! assert (double (pycall ("pyfunc", true)), 30)
%! assert (double (pycall ("pyfunc", false)), 20)
%! assert (double (pycall ("pyfunc", 10)), 10)

%!error <NameError>
%! pyexec ("def raiseException(): raise NameError('oops')")
%! pycall ("raiseException")

## None as a return value
%!test
%! f = pyeval ("lambda: None");
%! r = pycall (f);
%! assert (__py_is_none__ (r))

## But returning None will not set "ans"
%!test
%! f = pyeval ("lambda: None");
%! clear ans
%! pycall (f);
%! assert (! exist ("ans", "var"))
*/
