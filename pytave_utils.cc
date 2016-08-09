/*

Copyright (C) 2016 Abhinav Tripathi

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
#include <octave/parse.h>

#include "pytave_utils.h"

using namespace boost::python;

namespace pytave
{
  void get_builtins_module (boost::python::object& builtins_module)
  {
#if PY_VERSION_HEX >= 0x03000000
    builtins_module = import ("builtins");
#else
    builtins_module = import ("__builtin__");
#endif
  }

  void get_object_from_python (const octave_value& oct_value,
                               boost::python::object& py_object)
  {
    bool by_name;
    if (oct_value.is_string ())
      by_name = true;
    else if (oct_value.is_object () && oct_value.class_name () == "pyobject")
      by_name = false;
    else
      {
        py_object = boost::python::object();      //None
        return;
      }

    object main_module = import ("__main__");
    object builtins_module;
    get_builtins_module (builtins_module);

    if (by_name)
      {
        std::string module;
        std::string func = oct_value.string_value ();

        size_t idx = func.rfind (".");
        if (idx != std::string::npos)
          {
            module = func.substr (0, idx);
            func = func.substr (idx + 1);
          }

        object mod;
        if (module.empty ())
          {
            if (PyObject_HasAttrString (main_module.ptr (), func.c_str ()))
              mod = main_module;
            else
              mod = builtins_module;
          }
        else
          mod = import (module.c_str ());
        py_object = mod.attr (func.c_str ());
      }
    else
      {
        octave_value_list tmp = feval ("getid", ovl (oct_value), 1);
        std::string hexid = tmp(0).string_value ();
        py_object = main_module.attr ("__InOct__")[hexid];
      }
  }
}
