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

#define PYTAVE_DO_DECLARE_SYMBOL
#include "arrayobjectdefs.h"
#include "exceptions.h"
#include "python_to_octave.h"

using namespace boost::python;

DEFUN_DLD (pyexec, args, nargout,
           "-*- texinfo -*-\n\
@deftypefn {} {} pyexec (@var{expr})\n\
Execute a Python expression or block of code.\n\
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

  if (nargin != 1)
    {
      print_usage ();
      return retval;
    }

  std::string code = args(0).string_value ();

  Py_Initialize ();

  object main_module = import ("__main__");
  object main_namespace = main_module.attr ("__dict__");

  try
    {
      // FIXME: figure out exec return code:
      // http://www.boost.org/doc/libs/1_38_0/libs/python/doc/v2/exec.html
      exec (code.c_str (), main_namespace, main_namespace);
    }
  catch (pytave::object_convert_exception const &)
    {
      error ("pyexec: error in return value type conversion");
    }
  catch (error_already_set const &)
    {
      PyObject *ptype, *pvalue, *ptraceback;
      PyErr_Fetch (&ptype, &pvalue, &ptraceback);

      try
        {
          std::string message = extract<std::string> (pvalue);
          error ("pyexec: %s", message.c_str ());
        }
      catch (error_already_set const &)
        {
          PyErr_Restore (ptype, pvalue, ptraceback);
          PyErr_Print ();
        }
    }
  return retval;
}
