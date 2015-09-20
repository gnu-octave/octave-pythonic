/*

Copyright (C) 2015 Mike Miller

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

DEFUN_DLD (py, args, nargout,
           "-*- texinfo -*-\n\
@deftypefn {Loadable Function} py (@var{func})\n\
@enddeftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin != 1)
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

  try
    {
      object main_module = import ("__main__");
      object main_namespace = main_module.attr ("__dict__");

      object mod = (module.empty ()) ? main_module : import (module.c_str ());
      object callable = mod.attr (func.c_str ());
      object res = callable ();

      if (! res.is_none ())
        {
          octave_value val;
          pytave::pyobj_to_octvalue (val, res);
          retval(0) = val;
        }
    }
  catch (pytave::object_convert_exception const &)
    {
      error ("py: error in return value type conversion");
    }
  catch (error_already_set const &)
    {
      std::cerr << "in here" << std::endl;
      PyObject *ptype, *pvalue, *ptraceback;
      PyErr_Fetch (&ptype, &pvalue, &ptraceback);

      try
        {
          std::string message = extract<std::string> (pvalue);
          error ("py: %s", message.c_str ());
        }
      catch (error_already_set const &)
        {
          PyErr_Restore (ptype, pvalue, ptraceback);
          PyErr_Print ();
        }
    }

  return retval;
}

