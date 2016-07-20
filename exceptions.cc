/*

Copyright (C) 2015-2016 Mike Miller
Copyright (C) 2008 David Grundberg, Håkan Fors Nilsson

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
#include "exceptions.h"

namespace pytave
{

  PyObject *octave_error_exception::excclass = 0;
  PyObject *value_convert_exception::excclass = 0;
  PyObject *object_convert_exception::excclass = 0;
  PyObject *octave_parse_exception::excclass = 0;
  PyObject *variable_name_exception::excclass = 0;

  bool init_exceptions (void)
  {
    return (octave_error_exception::init ()
            && value_convert_exception::init ()
            && object_convert_exception::init ()
            && octave_parse_exception::init ()
            && variable_name_exception::init ());
  }

  std::string fetch_exception_message (void)
  {
    using namespace boost::python;
    PyObject *ptype, *pvalue, *ptraceback;
    PyErr_Fetch (&ptype, &pvalue, &ptraceback);
    std::string message;

    try
      {
        object formatted_list, formatted;
        handle<> htype (ptype), hval (allow_null (pvalue));
        object traceback (import ("traceback"));
        object format_exception_only (traceback.attr ("format_exception_only"));
        formatted_list = format_exception_only (htype, hval);
        formatted = str ("\n").join (formatted_list);
        message = extract<std::string> (formatted);
      }
    catch (error_already_set const &)
      {
        PyErr_Restore (ptype, pvalue, ptraceback);
        PyErr_Print ();
        message = std::string ("Something weird happened. See traceback above ^");
      }
    return message;
  }
}
