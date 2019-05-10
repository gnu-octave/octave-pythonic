/*

SPDX-License-Identifier: GPL-3.0-or-later

Copyright (C) 2016 Colin B. Macdonald
Copyright (C) 2016-2019 Mike Miller

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
@c FIXME: Python stdout not same as Octave's https://gitlab.com/mtmiller/octave-pythonic/issues/7\n\
@c doctest: +XFAIL\n\
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

  pythonic::py_init ();

  PyObject *local_namespace = nullptr;
  if (nargin > 1)
    {
      local_namespace = pythonic::pyobject_unwrap_object (args(1));
      if (! local_namespace)
        error ("pyexec: NAMESPACE must be a valid Python reference");
    }

  // FIXME: figure out exec return code:
  pythonic::py_exec_string (code, 0, local_namespace);

  return retval;
}

/*
%!error <NameError>
%! pyexec ("raise NameError ('oops')")

%!error <AttributeError>
%! pyexec ("import sys")
%! pyexec ("sys.no_such_thing")
*/
