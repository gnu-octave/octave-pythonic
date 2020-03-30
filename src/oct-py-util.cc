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
#include <vector>
#include <octave/oct-map.h>
#include <octave/oct.h>
#include <octave/parse.h>

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
  py_objstore_clear ()
  {
    python_object store = py_objstore ();
    PyDict_Clear (store);
    store.release ();
  }

  octave_map
  py_objstore_list ()
  {
    python_object store = py_objstore ();

    std::vector<std::string> fields { "key", "count", "type", "value" };

    Py_ssize_t sz = PyDict_Size (store);

    octave_map map { dim_vector (sz, 1), string_vector (fields) };

    octave_idx_type idx = 0;
    Py_ssize_t pos = 0;
    PyObject *key_obj, *tuple;

    while (PyDict_Next (store, &pos, &key_obj, &tuple))
      {
        if (! tuple || ! PyTuple_Check (tuple))
          continue;

        uint64_t key = PyLong_AsUnsignedLong (key_obj);
        uint64_t count = PyLong_AsUnsignedLong (PyTuple_GetItem (tuple, 0));
        PyObject *value = PyTuple_GetItem (tuple, 1);

        PyObject *valtype = PyObject_Type (value);
        PyObject *valtypename = PyObject_Str (PyObject_GetAttrString (valtype, "__name__"));
#if PY_VERSION_HEX >= 0x03000000
        std::string valtypestr = PyUnicode_AsUTF8 (valtypename);
#else
        std::string valtypestr = PyString_AsString (valtypename);
#endif
        Py_DECREF (valtype);
        Py_DECREF (valtypename);

        std::string s;
        if (false) // TODO http://gitlab.com/mtmiller/octave-pythonic/issues/51
          s = "<large object>";
        else
          {
            PyObject *valuestr = PyObject_Str (value);
            if (! valuestr)
              s = "<failed to extract string>";
            else
              {
#if PY_VERSION_HEX >= 0x03000000
                s = PyUnicode_AsUTF8 (valuestr);
#else
                s = PyString_AsString (valuestr);
#endif
                Py_DECREF (valuestr);
                if (s.empty ())
                  s = "<failed to extract string>";
                if (s.length() > 1000)
                  s = s.substr (0, 1000-3) + "...";
              }
          }

        octave_scalar_map entry { string_vector (fields) };
        entry.setfield ("key", octave_uint64 (key));
        entry.setfield ("count", octave_uint64 (count));
        entry.setfield ("type", valtypestr);
        entry.setfield ("value", s);
        map.fast_elem_insert (idx++, entry);
      }

    store.release ();

    return map;
  }

  void
  py_objstore_drop (uint64_t key)
  {
    python_object store = py_objstore ();
    python_object key_obj = make_py_int (key);
    if (PyDict_Contains (store, key_obj))
      {
        PyObject *tuple = PyDict_GetItem (store, key_obj);
        if (tuple && PyTuple_Check (tuple))
          {
            uint64_t count = PyLong_AsLong (PyTuple_GetItem (tuple, 0));
            //octave_stdout << "objstore debug: deleting key " << key << " w/ count " << count << " and erasing refcount" << std::endl;
            if (count > 1)
              {
                PyObject *obj = PyTuple_GetItem (tuple, 1);
                tuple = PyTuple_Pack (2, make_py_int (count - 1), obj);
                PyDict_SetItem (store, key_obj, tuple);
                Py_DECREF (tuple);
              }
            else
              PyDict_DelItem (store, key_obj);
          }
      }
    else
      {
        //octave_stdout << "objstore debug: asked to delete key " << key << " but its not present" << std::endl;
        // FIXME: surely this is an error?
      }
    store.release ();
  }

  PyObject *
  py_objstore_get (uint64_t key)
  {
    python_object store = py_objstore ();
    python_object key_obj = make_py_int (key);
    PyObject *tuple = PyDict_GetItem (store, key_obj);
    PyObject *obj = nullptr;
    if (tuple && PyTuple_Check (tuple))
      obj = PyTuple_GetItem (tuple, 1);
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
    if (PyDict_Contains (store, key_obj))
      {
        PyObject *tuple = PyDict_GetItem (store, key_obj);
        if (tuple && PyTuple_Check (tuple))
          {
            uint64_t count = PyLong_AsLong (PyTuple_GetItem (tuple, 0));
            tuple = PyTuple_Pack (2, make_py_int (count + 1), obj);
            PyDict_SetItem (store, key_obj, tuple);
            Py_DECREF (tuple);
          }
      }
    else
      {
        PyObject *tuple = PyTuple_Pack (2, make_py_int (1), obj);
        PyDict_SetItem (store, key_obj, tuple);
        Py_DECREF (tuple);
      }
    store.release ();
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
