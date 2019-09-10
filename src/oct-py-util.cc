/*

SPDX-License-Identifier: GPL-3.0-or-later

Copyright (C) 2016-2019 Mike Miller
Copyright (C) 2019 Colin B. Macdonald

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
#include <octave/parse.h>
#include <octave/Cell.h>

#include "oct-py-error.h"
#include "oct-py-object.h"
#include "oct-py-types.h"
#include "oct-py-util.h"

namespace pythonic
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
    PyObject *module = PyImport_ImportModule (name.c_str ());
    if (! module)
      PyErr_Clear ();
    return module;
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
  PyObject *objcount = nullptr;

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

  inline PyObject *
  py_objcount ()
  {
    if (! objcount)
      {
        python_object main = py_import_module ("__main__");
        python_object ns = main ? PyObject_GetAttrString (main, "__dict__") : 0;
        PyObject *dict = ns ? PyDict_GetItemString (ns, "_in_octave_count") : 0;

        if (dict)
          Py_INCREF (dict);

        if (! dict)
          {
            dict = PyDict_New ();
            if (dict && ns)
              PyDict_SetItemString (ns, "_in_octave_count", dict);
          }

        if (! dict)
          error_python_exception ();

        objcount = dict;
      }
    return objcount;
  }

  octave_value_list
  py_objstore_list ()
  {
    PyObject *key, *value;
    Py_ssize_t pos = 0;
    python_object store = py_objstore ();
    python_object count = py_objcount ();

    int sz = PyDict_Size (store);

    octave_value_list tmp = ovl();
    tmp.resize (sz);
    for (octave_idx_type i = 0; i < sz; i++) {
      tmp(i) = octave_value (0);
    }
    Cell c = tmp;
    octave_value_list retval;

    while (PyDict_Next (store, &pos, &key, &value)) {
      PyObject *keystrpy = PyObject_Str (key);
      PyObject *keylongpy = PyLong_FromUnicodeObject (keystrpy, 16);
      uint64_t keyi = PyLong_AsLong (keylongpy);
      Py_DECREF (keystrpy);
      Py_DECREF (keylongpy);

      PyObject *countobj = PyDict_GetItem (count, key);
      uint64_t counti = PyLong_AsLong (countobj);
      Py_DECREF (countobj);

      PyObject *valtype = PyObject_Type (value);
      PyObject *valtypename = PyObject_Str (PyObject_GetAttrString (valtype, "__name__"));
      std::string valtypestr = PyUnicode_AsUTF8 (valtypename);

      // TODO: should handle some errors here?
      PyObject *valuestr = PyObject_Str (value);
      std::string s = PyUnicode_AsUTF8 (valuestr);

      if (s.length() > 20)
        s = s.substr (0, 17) + "...";  // TODO: 19 and u8"…"?
      Cell c2 = ovl (octave_uint64 (keyi),      \
                     octave_uint64 (counti),    \
                     octave_value (valtypestr), \
                     octave_value (s));
      c.elem(pos-1) = c2;
      Py_DECREF (valtype);
      Py_DECREF (valtypename);
      Py_DECREF (valuestr);
    }
    store.release ();
    count.release ();
    retval(0) = c;
    return retval;
  }

  void
  py_objstore_del (uint64_t key)
  {
    python_object store = py_objstore ();
    python_object count = py_objcount ();
    python_object key_obj = make_py_int (key);
    python_object key_fmt = PyNumber_ToBase (key_obj, 16);
    if (PyDict_Contains (store, key_fmt)) {
      PyObject *tmpcountobj = PyDict_GetItem (count, key_fmt);
      uint64_t counti = PyLong_AsLong (tmpcountobj);
      Py_DECREF (tmpcountobj);
      //octave_stdout << "objstore debug: deleting key " << key << " w/ count " << counti << " and erasing refcount" << std::endl;
      PyDict_DelItem (store, key_fmt);
      PyDict_DelItem (count, key_fmt);
    } else {
      //octave_stdout << "objstore debug: asked to delete key " << key << " but its not present" << std::endl;
      // TODO: is this an error?
    }
    store.release ();
    count.release ();
  }

  PyObject *
  py_objstore_get (uint64_t key)
  {
    python_object store = py_objstore ();
    python_object count = py_objcount ();
    python_object key_obj = make_py_int (key);
    python_object key_fmt = PyNumber_ToBase (key_obj, 16);
    PyObject *obj = PyDict_GetItem (store, key_fmt);
    PyObject *tmpcountobj = PyDict_GetItem (count, key_fmt);
    uint64_t counti = PyLong_AsLong (tmpcountobj);
    Py_DECREF (tmpcountobj);
    //octave_stdout << "objstore debug: getting key " << key << ", incrementing count to " << counti + 1 << std::endl;
    PyDict_SetItem (count, key_fmt, make_py_int (counti+1));
    store.release ();
    count.release ();
    if (obj)
      Py_INCREF (obj);
    return obj;
  }

  uint64_t
  py_objstore_put (PyObject *obj)
  {
    python_object store = py_objstore ();
    python_object count = py_objcount ();
    uint64_t key = reinterpret_cast<uint64_t> (obj);
    python_object key_obj = make_py_int (key);
    python_object key_fmt = PyNumber_ToBase (key_obj, 16);
    if (PyDict_Contains (store, key_fmt)) {
      // TODO: we assume count and store never out of sync...
      PyObject *tmpcountobj = PyDict_GetItem (count, key_fmt);
      uint64_t counti = PyLong_AsLong (tmpcountobj);
      Py_DECREF (tmpcountobj);
      //octave_stdout << "objstore debug: key " << key << " already present with count " << counti << ", incrementing count to " << counti + 1 << std::endl;
      PyDict_SetItem (count, key_fmt, make_py_int (counti+1));
    } else {
      //octave_stdout << "objstore debug: adding new object with key " << key << std::endl;
      PyDict_SetItem (store, key_fmt, obj);
      PyDict_SetItem (count, key_fmt, make_py_int (0));
    }
    store.release ();
    count.release ();
    return key;
  }

  octave_value
  pyobject_wrap_object (PyObject *obj)
  {
    uint64_t key = py_objstore_put (obj);
    octave_value_list out = octave::feval ("pyobject", ovl (33554431.0, octave_uint64 (key)), 1);
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
