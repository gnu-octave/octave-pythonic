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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dlfcn.h>
#include <boost/python.hpp>
#include <boost/python/numeric.hpp>

/* Both boost::python and octave define HAVE_STAT and HAVE_FSTAT.  Ideally,
   they shouldn't expose their configuration in the header files, but they do.
   This silences the compiler warning. */
#undef HAVE_STAT
#undef HAVE_FSTAT

#include <oct.h>

#define PYTAVE_DO_DECLARE_SYMBOL
#include "pytavedefs.h"
#include "arrayobjectdefs.h"
#include "exceptions.h"
#include "python_to_octave.h"

using namespace boost::python;

DEFUN_DLD (pyeval, args, nargout,
           "-*- texinfo -*-\n\
@deftypefn  {Loadable Function} pyeval (@var{expr})\n\
Evaluate a python expression and return result.\n\
\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();

  std::string code = args(0).string_value ();

  dlopen("libpython2.7.so", RTLD_GLOBAL|RTLD_LAZY);


  Py_Initialize ();
  std::cerr << "python initialized" << std::endl;

  try
    {
      object main_module = import ("__main__");
      object main_namespace = main_module.attr ("__dict__");
      object res = eval (code.c_str (), main_namespace, main_namespace);
      // currently, we cannot return the raw object to octave...
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
      std::cerr << "in here" << std::endl;
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
      std::cerr << "leaving" << std::endl;
    }

  return retval;
}
