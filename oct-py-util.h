/*

Copyright (C) 2016 Mike Miller

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

#if ! defined (pytave_oct_py_util_h)
#define pytave_oct_py_util_h 1

#include <Python.h>
#include <stdint.h>
#include <string>

class octave_value;

namespace pytave
{

  //! Return a reference to the builtins module.
  //!
  //! @return reference to the builtins module
  PyObject *
  py_builtins_module ();

  //! Return a reference to the named function in the given module.
  //!
  //! @param module module to find the function in
  //! @param name name of the function
  //! @return a reference to the function, or a null pointer
  PyObject *
  py_find_function (PyObject *module, const std::string& name);

  //! Return a reference to the named function in the given module.
  //!
  //! @param module name of the module to find the function in
  //! @param name name of the function
  //! @return a reference to the function, or a null pointer
  PyObject *
  py_find_function (const std::string& module, const std::string& name);

  //! Return a reference to the fully-qualified function name.
  //!
  //! @param name fully-qualified name of the function
  //! @return a reference to the function, or a null pointer
  PyObject *
  py_find_function (const std::string& name);

  //! Return a reference to the fully-qualified type name.
  //!
  //! @param name fully-qualified name of the type
  //! @return a reference to the type, or a null pointer
  PyObject *
  py_find_type (const std::string& name);

  //! Return a reference to the named module.
  //!
  //! @param name fully-qualified name of the module
  //! @return a reference to the module, or a null pointer
  PyObject *
  py_import_module (const std::string& name);

  //! Check whether an object is an instance of a type.
  //!
  //! @param obj Python object
  //! @param type Python type
  //! @return @c true if @a obj is an instance of @a type, @c false otherwise
  bool
  py_isinstance (PyObject *obj, PyObject *type);

  std::string
  py_object_class_name (PyObject *obj);

  void
  py_objstore_del (uint64_t key);

  PyObject *
  py_objstore_get (uint64_t key);

  uint64_t
  py_objstore_put (PyObject *obj);

  octave_value
  pyobject_wrap_object (PyObject *obj);

  PyObject *
  pyobject_unwrap_object (const octave_value& value);

  bool
  is_py_kwargs_argument (PyObject *obj);

  PyObject *
  update_py_dict (PyObject *dict_orig, PyObject *dict_new);

}

#endif
