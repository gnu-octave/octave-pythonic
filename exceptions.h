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

#if ! defined (pytave_exceptions_h)
#define pytave_exceptions_h

#include <Python.h>
#include <string>

namespace pytave
{
  class pytave_exception
  {
  public:
    static void translate_exception (const pytave_exception& py_ex)
    {
      PyErr_SetString (PyExc_Exception, py_ex.error.c_str ());
    }

    pytave_exception (const std::string& err) { error = err; };

  private:
    std::string error;
  };

  class octave_error_exception
  {
  public:
    static bool init ()
    {
      excclass = PyErr_NewException (const_cast<char*> ("pytave.OctaveError"),
                                     PyExc_RuntimeError, 0);
      return (excclass != 0);
    };

    static void translate_exception (const octave_error_exception& py_ex)
    {
      PyErr_SetString (excclass, py_ex.error.c_str ());
    }

    static PyObject *excclass;

    octave_error_exception (const std::string& err) { error = err; };

  private:
    std::string error;
  };

  class octave_parse_exception
  {
  public:
    static bool init ()
    {
      excclass = PyErr_NewException (const_cast<char*> ("pytave.ParseError"),
                                     PyExc_RuntimeError, 0);
      return (excclass != 0);
    };

    static void translate_exception (const octave_parse_exception& py_ex)
    {
      PyErr_SetString (excclass, py_ex.error.c_str ());
    }

    static PyObject *excclass;

    octave_parse_exception (const std::string& err) { error = err; };

  private:
    std::string error;
  };

  class value_convert_exception
  {
  public:
    static bool init ()
    {
      excclass = PyErr_NewException (const_cast<char*> ("pytave.ValueConvertError"),
                                     PyExc_TypeError, 0);
      return (excclass != 0);
    };

    static void translate_exception (const value_convert_exception& py_ex)
    {
      PyErr_SetString (excclass, py_ex.error.c_str ());
    }

    static PyObject *excclass;

    value_convert_exception (const std::string& err) { error = err; };

  private:
    std::string error;
  };

  class object_convert_exception
  {
  public:
    static bool init ()
    {
      excclass = PyErr_NewException (const_cast<char*> ("pytave.ObjectConvertError"),
                                     PyExc_TypeError, 0);
      return (excclass != 0);
    };

    static void translate_exception (const object_convert_exception& py_ex)
    {
      PyErr_SetString (excclass, py_ex.error.c_str ());
    }

    static PyObject *excclass;

    object_convert_exception (const std::string& err) { error = err; };

  private:
    std::string error;
  };

  class variable_name_exception
  {
  public:
    static bool init ()
    {
      excclass = PyErr_NewException (const_cast<char*> ("pytave.VarNameError"),
                                     PyExc_RuntimeError, 0);
      return (excclass != 0);
    };

    static void translate_exception (const variable_name_exception& py_ex)
    {
      PyErr_SetString (excclass, py_ex.error.c_str ());
    }

    static PyObject *excclass;

    variable_name_exception (const std::string& err) { error = err; };

  private:
    std::string error;
  };
}

#endif
