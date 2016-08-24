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

#include "oct-py-types.h"
#include "oct-py-util.h"

using namespace boost::python;

namespace pytave
{

void
get_builtins_module (boost::python::object& builtins_module)
{
#if PY_VERSION_HEX >= 0x03000000
  builtins_module = import ("builtins");
#else
  builtins_module = import ("__builtin__");
#endif
}

void
get_object_from_python (const octave_value& oct_value,
                        boost::python::object& py_object)
{
  if (oct_value.is_object () && oct_value.class_name () == "pyobject")
    {
      octave_value_list tmp = feval ("getid", ovl (oct_value), 1);
      std::string hexid = tmp(0).string_value ();
      object main_module = import ("__main__");
      py_object = main_module.attr ("_in_octave")[hexid];
    }
  else
    py_object = boost::python::object (); // None
}

PyObject *
py_builtins_module ()
{
#if PY_VERSION_HEX >= 0x03000000
  return py_import_module ("builtins");
#else
  return py_import_module ("__builtin__");
#endif
}

PyObject *
py_find_function (PyObject *module, const std::string& name)
{
  if (module && PyModule_Check (module))
    {
      PyObject *obj = PyObject_GetAttrString (module, name.c_str ());
      if (obj && ! PyCallable_Check (obj))
        {
          Py_CLEAR (obj);
        }

      return obj;
    }

  return 0;
}

PyObject *
py_find_function (const std::string& module, const std::string& name)
{
  PyObject *mod = py_import_module (module);
  PyObject *func =  py_find_function (mod, name);
  Py_XDECREF (mod);
  return func;
}

PyObject *
py_find_function (const std::string& name)
{
  std::string::size_type idx = name.rfind (".");
  if (idx == std::string::npos)
    {
      PyObject *func = py_find_function ("__main__", name);
      if (! func)
        func = py_find_function (py_builtins_module (), name);
      return func;
    }
  else
    {
      std::string module = name.substr (0, idx);
      std::string function = name.substr (idx + 1);
      return py_find_function (module, function);
    }
}

PyObject *
py_find_type (const std::string& name)
{
  PyObject *obj = py_find_function (name);
  if (obj && PyType_Check (obj))
    return obj;

  Py_XDECREF (obj);
  return 0;
}

PyObject *
py_import_module (const std::string& name)
{
  return PyImport_ImportModule (name.c_str ());
}

bool
py_isinstance (PyObject *obj, PyObject *type)
{
  if (obj && type)
    return static_cast<bool> (PyObject_IsInstance (obj, type));

  return false;
}

std::string
py_object_class_name (PyObject *obj)
{
  PyObject *class_ = obj ? PyObject_GetAttrString (obj, "__class__") : 0;
  PyObject *name_ = class_ ? PyObject_GetAttrString (class_, "__name__") : 0;
  return name_ ? extract_py_str (name_): "";
}

bool
is_py_kwargs_argument (PyObject *obj)
{
  if (obj && py_object_class_name (obj) == "_OctaveKwargs"
      && PyObject_HasAttrString (obj, "is_kwargs_argument"))
    {
      PyObject *flag = PyObject_GetAttrString (obj, "is_kwargs_argument");
      if (flag && PyBool_Check (flag) && PyObject_IsTrue (flag))
        return true;
    }
  return false;
}

PyObject *
update_py_dict (PyObject *dict_orig, PyObject *dict_new)
{
  PyObject *dict = dict_orig ? dict_orig : PyDict_New ();
  PyDict_Update (dict, dict_new);
  return dict;
}

}
