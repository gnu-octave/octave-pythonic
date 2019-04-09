/*

Copyright (C) 2016-2019 Mike Miller

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

#include <Python.h>
#include <octave/oct.h>
#include <octave/parse.h>

#include "oct-py-error.h"
#include "oct-py-object.h"
#include "oct-py-types.h"
#include "oct-py-util.h"

namespace pytave
{

  inline std::string
  py_builtins_module_name ()
  {
#if PY_VERSION_HEX >= 0x03000000
    return "builtins";
#else
    return "__builtin__";
#endif
  }

  PyObject *
  py_builtins_module ()
  {
    return py_import_module (py_builtins_module_name ());
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
    python_object mod = py_import_module (module);
    PyObject *func =  py_find_function (mod, name);
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
    python_object obj = py_find_function (name);
    if (obj && PyType_Check (obj))
      return obj.release ();

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
    std::string retval;

    python_object type = obj ? PyObject_GetAttrString (obj, "__class__") : 0;
    if (type)
      {
        python_object mod = PyObject_GetAttrString (type, "__module__");

        python_object name;
        if (PyObject_HasAttrString (type, "__qualname__"))
          name = PyObject_GetAttrString (type, "__qualname__");
        else
          name = PyObject_GetAttrString (type, "__name__");

        std::string mod_str = !mod.is_none () ? extract_py_str (mod) : "";
        std::string name_str = name ? extract_py_str (name) : "";

        if (mod_str.empty () || mod_str == py_builtins_module_name ())
          retval = name_str;
        else
          retval = mod_str + "." + name_str;
      }

    return retval;
  }

  // FIXME: could make this into a class/singleton wrapper a la Octave core
  PyObject *objstore = nullptr;

  inline PyObject *
  py_objstore ()
  {
    if (! objstore)
      {
        python_object main = py_import_module ("__main__");
        python_object ns = main ? PyObject_GetAttrString (main, "__dict__") : 0;
        PyObject *dict = ns ? PyDict_GetItemString (ns, "_in_octave") : 0;

        if (dict)
          Py_INCREF (dict);

        if (! dict)
          {
            dict = PyDict_New ();
            if (dict && ns)
              PyDict_SetItemString (ns, "_in_octave", dict);
          }

        if (! dict)
          error_python_exception ();

        objstore = dict;
      }
    return objstore;
  }

  void
  py_objstore_del (uint64_t key)
  {
    python_object store = py_objstore ();
    python_object key_obj = make_py_int (key);
    python_object key_fmt = PyNumber_ToBase (key_obj, 16);
    PyDict_DelItem (store, key_fmt);
    store.release ();
  }

  PyObject *
  py_objstore_get (uint64_t key)
  {
    python_object store = py_objstore ();
    python_object key_obj = make_py_int (key);
    python_object key_fmt = PyNumber_ToBase (key_obj, 16);
    PyObject *obj = PyDict_GetItem (store, key_fmt);
    store.release ();
    if (obj)
      Py_INCREF (obj);
    return obj;
  }

  uint64_t
  py_objstore_put (PyObject *obj)
  {
    python_object store = py_objstore ();
    uint64_t key = reinterpret_cast<uint64_t> (obj);
    python_object key_obj = make_py_int (key);
    python_object key_fmt = PyNumber_ToBase (key_obj, 16);
    PyDict_SetItem (store, key_fmt, obj);
    store.release ();
    return key;
  }

  octave_value
  pyobject_wrap_object (PyObject *obj)
  {
    uint64_t key = py_objstore_put (obj);
    octave_value_list out = octave::feval ("pyobject", ovl (0, octave_uint64 (key)), 1);
    return out(0);
  }

  PyObject *
  pyobject_unwrap_object (const octave_value& value)
  {
    if (value.isobject () && value.class_name () == "pyobject")
      {
        octave_value_list out = octave::feval ("id", ovl (value), 1);
        uint64_t key = out(0).uint64_scalar_value ();
        return py_objstore_get (key);
      }

    return 0;
  }

  bool
  is_py_kwargs_argument (PyObject *obj)
  {
    if (obj && py_object_class_name (obj) == "__main__._OctaveKwargs"
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
