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

  Py_Initialize ();

  try
    {
      object main_module = import ("__main__");
      object main_namespace = main_module.attr ("__dict__");
      object res = eval (code.c_str (), main_namespace, main_namespace);

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
      error ("pyeval: error in return value type conversion");
    }
  catch (error_already_set const &)
    {
      PyObject *ptype, *pvalue, *ptraceback;
      PyErr_Fetch (&ptype, &pvalue, &ptraceback);

      try
        {
          std::string message = extract<std::string> (pvalue);
          error ("pyeval: %s", message.c_str ());
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
%!test
%! q = pyeval ('10.1')
%! assert (isnumeric(q))
%! % note: floating-point equality test: usually bad but here we expect the exact same float
%! assert (q, 10.1)

%!test
%! q = pyeval ('"I <3 Octave"')
%! assert (ischar (q))
%! assert (~strcmp (q, '1 <3 Octave'))

%!test
%! % This might change if we stop converting lists
%! z = pyeval ('[1, [21, 22], 3, [41, [421, 422], 43]]');
%! assert (z{2}{1} == 21)
%! assert (z{2}{2} == 22)
%! assert (z{4}{2}{1} == 421)
%! assert (z{4}{2}{2} == 422)
*/
