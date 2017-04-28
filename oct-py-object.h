/*

Copyright (C) 2017 Abhinav Tripathi

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

#if ! defined (pytave_oct_py_object_h)
#define pytave_oct_py_object_h

#include <Python.h>

namespace pytave
{

  class python_object
  {
  public:
    python_object (PyObject *obj = 0)
    {
      pyobj = obj;
      isowned = pyobj != 0;
    }

    python_object (const python_object& oth)
    {
      pyobj = oth.pyobj;
      isowned = oth.isowned;
      if (isowned)
        Py_INCREF (pyobj);
    }

    ~python_object ()
    {
      if (isowned)
        Py_DECREF (pyobj);
    }

    python_object& operator= (const python_object& oth)
    {
      if (isowned)
        Py_DECREF (pyobj);
      pyobj = oth.pyobj;
      isowned = oth.isowned;
      if (isowned)
        Py_INCREF (pyobj);
      return *this;
    }

    python_object& operator= (PyObject *obj)
    {
      if (isowned)
        Py_DECREF (pyobj);
      pyobj = obj;
      isowned = pyobj != 0;
      if (isowned)
        Py_INCREF (pyobj);
      return *this;
    }

    operator bool () const
    {
      return isowned;
    }

    operator PyObject *()
    {
      return pyobj;
    }

    bool is_none ()
    {
      return pyobj && pyobj == Py_None;
    }

    PyObject *release ()
    {
      isowned = false;
      PyObject *ret = pyobj;
      pyobj = 0;
      return ret;
    }

  private:
    PyObject *pyobj;
    bool isowned;
  };

}

#endif
