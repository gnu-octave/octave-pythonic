/*

Copyright (C) 2016 Colin B. Macdonald

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

#include <dlfcn.h>
#include <boost/python.hpp>
#include <boost/python/numeric.hpp>

#include <oct.h>
#include <octave/parse.h>

#define PYTAVE_DO_DECLARE_SYMBOL
#include "arrayobjectdefs.h"
#include "exceptions.h"
#include "oct-py-util.h"
#include "python_to_octave.h"

using namespace boost::python;

DEFUN_DLD (pyeval, args, nargout,
           "-*- texinfo -*-\n\
@deftypefn  {} {} pyeval (@var{expr})\n\
@deftypefnx {} {} pyeval (@var{expr}, @var{localns})\n\
@deftypefnx {} {@var{x} =} pyeval (@dots{})\n\
Evaluate a Python expression and return the result.\n\
\n\
When called with an optional second argument, @var{localns} is a\n\
@code{py.dict} that acts as the namespace for any assignments or other\n\
side effects of the expression.\n\
\n\
Examples:\n\
@example\n\
@group\n\
pyexec (\"import sys\")\n\
pyeval (\"sys.version\")\n\
  @result{} ...\n\
pyeval (\"dict(two=2)\")\n\
  @result{} [pyobject ...]\n\
      @{'two': 2@}\n\
@end group\n\
@end example\n\
@seealso{pycall, pyexec}\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin < 1 || nargin > 2)
    {
      print_usage ();
      return retval;
    }

  std::string code = args(0).string_value ();

  std::string id;
  object res;

  Py_Initialize ();

  object main_module = import ("__main__");
  object main_namespace = main_module.attr ("__dict__");
  object local_namespace;
  if (nargin > 1)
  {
    pytave::get_object_from_python (args(1), local_namespace);
    if (local_namespace.is_none ())
      error ("pyeval: NAMESPACE must be a string or a Python reference");
  }
  else
    local_namespace = main_namespace;

  try
    {
      res = eval (code.c_str (), main_namespace, local_namespace);

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
  catch (error_already_set const &)
    {
      std::string message = pytave::fetch_exception_message ();
      error ("pyeval: %s", message.c_str ());
    }

  return retval;
}

/*
%!assert (isnumeric (double (pyeval ("0"))))
%!assert (isreal (double (pyeval ("0"))))
%!assert (double (pyeval ("0")), 0)

%!assert (isnumeric (pyeval ("10.1")))
%!assert (isreal (pyeval ("10.1")))
%!assert (pyeval ("10.1"), 10.1)

%!assert (isnumeric (pyeval ("2j")))
%!assert (iscomplex (pyeval ("2j")))
%!assert (pyeval ("2j"), 2j)

%!assert (ischar (pyeval ("\"I <3 Octave\"")))
%!assert (pyeval ("\"I <3 Octave\""), "I <3 Octave")

%!assert (islogical (pyeval ("True")))
%!assert (islogical (pyeval ("False")))
%!assert (pyeval ("True"), true)
%!assert (pyeval ("False"), false)
%!assert (class (pyeval ("True")), "logical")
%!assert (class (pyeval ("False")), "logical")

%!assert (isa (pyeval ("object()"), "pyobject"))

%!assert (isnumeric (double (pyeval ("__import__('sys').maxsize"))))
%!assert (double (pyeval ("99999999999999")), 99999999999999)
%!assert (double (pyeval ("-99999999999999")), -99999999999999)

%!test
%! z = pyeval ("{'x': 1., 'y': 2.}");
%! assert (isa (z, "pyobject"))
%! assert (z{"x"}, 1)

%!test
%! z = pyeval ("[1., 2., 3.]");
%! assert (isa (z, "pyobject"))
%! assert ({z{1}, z{2}, z{3}}, {1, 2, 3})

%!test
%! z = pyeval ("(4., 5., 6.)");
%! assert (isa (z, "pyobject"))
%! assert ({z{1}, z{2}, z{3}}, {4, 5, 6})

%!test
%! z = pyeval ("[1., [21., 22.], 3., [41., [421., 422.], 43.]]");
%! assert (isa (z, "pyobject"))
%! assert (isa (z{2}, "pyobject"))
%! assert (z{2}{1}, 21)
%! assert (z{2}{2}, 22)
%! assert (isa (z{4}{2}, "pyobject"))
%! assert (z{4}{2}{1}, 421)
%! assert (z{4}{2}{2}, 422)

%!error <NameError>
%! pyexec ("def raiseException(): raise NameError ('oops')")
%! pyeval ("raiseException()")

%!test
%! % Variable defined in global namespace is available locally
%! myNS = pyeval ("{}");
%! pyexec ("myvar = 1.")
%! assert (pyeval ("myvar", myNS), 1);

%!test
%! % Variables with same name can have different values in different namespaces
%! myNS1 = pyeval ("{}");
%! myNS2 = pyeval ("{}");
%! pyexec ("myvar = 1.")
%! pyexec ("myvar = 2.", myNS1)
%! pyexec ("myvar = 3.", myNS2)
%! assert (pyeval ("myvar"), 1)
%! assert (pyeval ("myvar", myNS1), 2)
%! assert (pyeval ("myvar", myNS2), 3)

%!test
%! pyexec ("if 'myvar' in globals(): del myvar")
%! % Namespaces can also be passed as strings
%! pyexec ("myNS = {}");
%! pyexec ("myvar = 1.", "myNS");
%! assert (pyeval ("myvar", "myNS"), 1);

%!error <NameError>
%! pyexec ("if 'myvar' in globals(): del myvar")
%! % Variable defined in local namespace MUST not be available globally
%! myNS = pyeval ("{}");
%! pyexec ("myvar = 1.", myNS)
%! pyeval ("myvar");

%!error <NameError>
%! pyexec ("if 'myvar' in globals(): del myvar")
%! % Variable defined in one local namespace MUST not be available in another
%! myNS1 = pyeval ("{}");
%! myNS2 = pyeval ("{}");
%! pyexec ("myvar = 1.", myNS1)
%! pyeval ("myvar", myNS2);

%!error <NameError>
%! pyexec ("if 'sys' in globals(): del sys")
%! % Modules imported in local namespace MUST not be accessible globally
%! myNS = pyeval ("{}");
%! pyexec ("import sys", myNS);
%! pyeval ("sys");
*/
