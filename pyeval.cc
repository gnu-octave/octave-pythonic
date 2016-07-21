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
#include "python_to_octave.h"

using namespace boost::python;

DEFUN_DLD (pyeval, args, nargout,
           "-*- texinfo -*-\n\
@deftypefn  {} {} pyeval (@var{expr})\n\
@deftypefnx {} {@var{x} =} pyeval (@var{expr})\n\
Evaluate a Python expression and return the result.\n\
\n\
Examples:\n\
@example\n\
@group\n\
pyexec (\"import sys\")\n\
pyeval (\"sys.version\")\n\
  @result{} ...\n\
pyeval (\"dict(one=1, two=2)\")\n\
  @result{} scalar structure containing the fields:\n\
      two =  2\n\
      one =  1\n\
@end group\n\
@end example\n\
@seealso{pycall, pyexec}\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();

  std::string code = args(0).string_value ();

  std::string id;
  object res;

  Py_Initialize ();

  object main_module = import ("__main__");
  object main_namespace = main_module.attr ("__dict__");
#if PY_VERSION_HEX >= 0x03000000
  object builtins_module = import ("builtins");
#else
  object builtins_module = import ("__builtin__");
#endif

  try
    {
      res = eval (code.c_str (), main_namespace, main_namespace);
      // hex(id(res))
      object hex_function = builtins_module.attr ("hex");
      object id_function = builtins_module.attr ("id");
      object idtmp = hex_function (id_function (res));
      id = extract<std::string> (idtmp);

      // FIXME: currently, we cannot return the raw object to octave...
      if (! res.is_none ())
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
  catch (error_already_set const &)
    {
      std::string message = pytave::fetch_exception_message ();
      error ("pyeval: %s", message.c_str ());
    }

  return retval;
}

/*
%!assert (isnumeric (pyeval ("0")))
%!assert (isreal (pyeval ("0")))
%!assert (pyeval ("0"), 0)

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

## FIXME: these will change when dict, list, and tuple are not converted
%!assert (pyeval ("{'x': 1, 'y': 2}"), struct ("x", 1, "y", 2))
%!assert (pyeval ("[1, 2, 3]"), {1, 2, 3})
%!assert (pyeval ("(4, 5, 6)"), {4, 5, 6})

%!test
%! % FIXME: this will change when we stop converting lists
%! z = pyeval ("[1, [21, 22], 3, [41, [421, 422], 43]]");
%! assert (z{2}{1}, 21)
%! assert (z{2}{2}, 22)
%! assert (z{4}{2}{1}, 421)
%! assert (z{4}{2}{2}, 422)

%!error <NameError>
%! pyexec ("def raiseException ():\n  raise NameError ('oops')")
%! pyeval ("raiseException ()")
*/
