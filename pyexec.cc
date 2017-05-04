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

#include <oct.h>

#include "exceptions.h"
#include "oct-py-eval.h"
#include "oct-py-init.h"
#include "oct-py-util.h"

DEFUN_DLD (pyexec, args, ,
           "-*- texinfo -*-\n\
@deftypefn  {} {} pyexec (@var{expr})\n\
@deftypefnx {} {} pyexec (@var{expr}, @var{localns})\n\
Execute a Python expression or block of code.\n\
\n\
When called with an optional second argument, @var{localns} is a\n\
@code{py.dict} that acts as the namespace for any assignments or other\n\
side effects of the expression.\n\
\n\
Examples:\n\
@example\n\
@group\n\
pyexec (\"print(42)\")\n\
  @print{} 42\n\
@end group\n\
@end example\n\
@seealso{pycall, pyeval}\n\
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

  pytave::py_init ();

  PyObject *local_namespace = nullptr;
  if (nargin > 1)
    {
      local_namespace = pytave::pyobject_unwrap_object (args(1));
      if (! local_namespace)
        error ("pyexec: NAMESPACE must be a valid Python reference");
    }

  try
    {
      // FIXME: figure out exec return code:
      pytave::py_exec_string (code, 0, local_namespace);
    }
  catch (pytave::object_convert_exception const &)
    {
      error ("pyexec: error in return value type conversion");
    }
  catch (pytave::error_already_set const &)
    {
      std::string message = pytave::fetch_exception_message ();
      error ("pyexec: %s", message.c_str ());
    }
  return retval;
}

/*
%!error <NameError>
%! pyexec ("raise NameError ('oops')")

%!error <AttributeError>
%! pyexec ("import sys")
%! pyexec ("sys.no_such_thing")
*/
