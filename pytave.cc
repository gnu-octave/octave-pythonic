/*

Copyright (C) 2015-2016 Mike Miller
Copyright (C) 2008 David Grundberg, Håkan Fors Nilsson
Copyright (C) 2009 Jaroslav Hajek, VZLU Prague

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
#include <boost/python/numeric.hpp>

#include <octave/oct.h>
#include <octave/oct-map.h>
#include <octave/octave.h>
#include <octave/ov.h>
#include <octave/parse.h>
#include <octave/symtab.h>
#include <octave/toplev.h>
#include <octave/utils.h>
#if defined (HAVE_OCTAVE_CALL_STACK_H)
#  include <octave/call-stack.h>
#endif

#include <iostream>
#ifdef HAVE_USELOCALE
#include <locale.h>
#endif
#include <sstream>
#include <sys/types.h>

#define PYTAVE_DO_DECLARE_SYMBOL
#include "arrayobjectdefs.h"
#include "exceptions.h"
#include "octave_to_python.h"
#include "python_to_octave.h"

using namespace boost::python;

namespace pytave
{

#ifdef HAVE_USELOCALE
  locale_t c_locale;
#endif

#if defined (PYTHON_ABI_VERSION)
  PyObject *
#else
  void
#endif
  init (bool silent = true)
  {
#ifdef HAVE_USELOCALE
    c_locale = newlocale (LC_ALL, "C", 0);
#endif

    if (! init_exceptions ())
      {
        PyErr_SetString (PyExc_ImportError, "_pytave: init failed");
#if defined (PYTHON_ABI_VERSION)
        return 0;
#else
        return;
#endif
      }

    // Initialize Octave.
    // Also print Octave startup message.
    int argc = 5;
    const char *argv[] =
    {
      "octave",
      "--no-line-editing",
      "--no-history",
      "--no-init-file",
      "--silent",
      0
    };

    if (silent)
      {
         argc--;
         argv[argc] = 0;
      }

#ifdef HAVE_USELOCALE
    // Set C locale
    locale_t old_locale = uselocale (c_locale);
#endif

    octave_main (argc, const_cast<char **> (argv), 1);

#ifdef HAVE_USELOCALE
    // Reset locale
    uselocale (old_locale);
#endif

    // Initialize NumPy Array

    // Let boost use numpy
    numeric::array::set_module_and_type ("numpy", "ndarray");

    // This is actually a macro that becomes a block expression. If an error
    // occurs, e.g. NumPy not installed, an exception is set.
    import_array ()
  }

  boost::python::tuple
  get_exceptions ()
  {
    return make_tuple (object (handle<PyObject> (
                                 octave_error_exception::excclass)),
                       object (handle<PyObject> (
                                 value_convert_exception::excclass)),
                       object (handle<PyObject> (
                                 object_convert_exception::excclass)),
                       object (handle<PyObject> (
                                 octave_parse_exception::excclass)),
                       object (handle<PyObject> (
                                 variable_name_exception::excclass)));
  }

  std::string
  make_error_message ()
  {
    std::ostringstream exceptionmsg;

    std::string message = last_error_message ();
    std::string identifier = last_error_id ();

    // Trim trailing new lines
    message = message.substr (0, message.find_last_not_of ("\r\n") + 1);

    octave_map stack = last_error_stack ();

    if (! stack.is_empty ())
      {
        std::string file = stack(0).getfield ("file").string_value ();
        std::string name = stack(0).getfield ("name").string_value ();
        int line = stack(0).getfield ("line").int_value ();
        int column = stack(0).getfield ("column").int_value ();

        exceptionmsg << file << ":" << line << ":" << column << ": ";
        if (! name.empty ())
          exceptionmsg << "in '" << name << "': ";
      }

    if (! identifier.empty ())
      exceptionmsg << "(identifier: " << identifier << ") ";

    exceptionmsg << message;

    return exceptionmsg.str ();
  }

  boost::python::tuple
  func_eval (int nargout, const std::string& funcname, const boost::python::tuple& arguments)
  {
    octave_value_list octave_args, retval;

    pytuple_to_octlist (octave_args, arguments);

    reset_error_handler ();
    buffer_error_messages++;

    // Updating the timestamp makes Octave reread changed files
    Vlast_prompt_time.stamp ();

#ifdef HAVE_USELOCALE
    // Set C locale
    locale_t old_locale = uselocale (c_locale);
#endif

    bool bad_alloc_state = false;
    bool octave_error = false;

    Py_BEGIN_ALLOW_THREADS
    try
      {
        retval = feval (funcname, octave_args, (nargout >= 0) ? nargout : 0);
      }
    catch (std::bad_alloc)
      {
        bad_alloc_state = true;
      }
    catch (const octave_execution_exception&)
      {
        octave_error = true;
      }
    Py_END_ALLOW_THREADS

#ifdef HAVE_USELOCALE
    // Reset locale
    uselocale (old_locale);
#endif

    if (bad_alloc_state)
      throw std::bad_alloc (); // Translated to MemoryError by boost::python

    if (octave_error)
      {
        std::string exceptionmsg = make_error_message ();
        if (! exceptionmsg.empty ())
          throw octave_error_exception (exceptionmsg);
        else
          throw octave_error_exception ("No Octave error available");
      }

    if (nargout >= 0)
      {
        boost::python::tuple pytuple;
        octlist_to_pytuple (pytuple, retval);
        return pytuple;
      }
    else
      return make_tuple ();
  }

  boost::python::tuple
  str_eval (int nargout, const std::string& code, bool silent)
  {
    octave_value_list retval;
    int parse_status;

    reset_error_handler ();
    buffer_error_messages++;

    // Updating the timestamp makes Octave reread changed files
    Vlast_prompt_time.stamp ();

#ifdef HAVE_USELOCALE
    // Set C locale
    locale_t old_locale = uselocale (c_locale);
#endif

    bool bad_alloc_state = false;
    bool octave_error = false;

    Py_BEGIN_ALLOW_THREADS
    try
      {
        retval = eval_string (code, silent, parse_status, (nargout >= 0) ? nargout : 0);
      }
    catch (std::bad_alloc)
      {
        bad_alloc_state = true;
      }
    catch (const octave_execution_exception&)
      {
        octave_error = true;
      }
    Py_END_ALLOW_THREADS

#ifdef HAVE_USELOCALE
    // Reset locale
    uselocale (old_locale);
#endif

    if (bad_alloc_state)
      throw std::bad_alloc (); // Translated to MemoryError by boost::python

    if (octave_error || parse_status)
      {
        std::string exceptionmsg = make_error_message ();
        if (! exceptionmsg.empty ())
          {
            if (parse_status != 0)
              throw octave_parse_exception (exceptionmsg);
            else
              throw octave_error_exception (exceptionmsg);
          }
        else
          throw octave_error_exception ("No Octave error available");
      }

    if (nargout >= 0)
      {
        boost::python::tuple pytuple;
        octlist_to_pytuple (pytuple, retval);
        return pytuple;
      }
    else
      return make_tuple ();
  }

  boost::python::object
  getvar (const std::string& name, bool global)
  {
    octave_value val;

    if (global)
      val = symbol_table::global_varval (name);
    else
      val = symbol_table::varval (name);

    if (val.is_undefined ())
      throw variable_name_exception (name + " not defined in current scope");

    boost::python::object pyobject;
    octvalue_to_pyobj (pyobject, val);

    return pyobject;
  }

  void
  setvar (const std::string& name, const boost::python::object& pyobject, bool global)
  {
    octave_value val;

    if (! valid_identifier (name))
      throw variable_name_exception (name + " is not a valid identifier");

    pyobj_to_octvalue (val, pyobject);

    if (global)
      symbol_table::global_assign (name, val);
    else
      symbol_table::assign (name, val);
  }

  bool
  isvar (const std::string& name, bool global)
  {
    bool retval;

    if (global)
      retval = symbol_table::global_varval (name).is_defined ();
    else
      retval = symbol_table::is_variable (name);

    return retval;
  }

  void
  delvar (const std::string& name, bool global)
  {
    if (global)
      symbol_table::clear_global (name);
    else
      symbol_table::clear_variable (name);
  }

  int
  push_scope ()
  {
    symbol_table::scope_id local_scope = symbol_table::alloc_scope ();
    symbol_table::set_scope (local_scope);
    octave_call_stack::push (local_scope);
    return local_scope;
  }

  void
  pop_scope ()
  {
    symbol_table::scope_id curr_scope = symbol_table::current_scope ();
    if (curr_scope != symbol_table::top_scope ())
      {
        symbol_table::erase_scope (curr_scope);
        octave_call_stack::pop ();
      }
  }

// Make sure Octave is correctly unloaded. We cannot depend on Octave running
// at the (true) process atexit point, because at that time the Octave library
// might have been unloaded.
//
// At least that is the hypothesis, since Octave (in certain circumstances)
// cause a segmentation fault in do_octave_atexit called from the exit
// function. (One Octave call that causes this is "sleep(0)".)
  void
  atexit ()
  {
#ifdef HAVE_USELOCALE
    // Set C locale
    locale_t old_locale = uselocale (c_locale);
#endif

    Py_BEGIN_ALLOW_THREADS
    clean_up_and_exit (0);
    Py_END_ALLOW_THREADS

#ifdef HAVE_USELOCALE
    // Reset locale
    uselocale (old_locale);
#endif
  }
}

BOOST_PYTHON_MODULE (_pytave)
{
  using namespace boost::python;

  def ("init", pytave::init);
  def ("feval", pytave::func_eval);
  def ("eval", pytave::str_eval);
  def ("getvar", pytave::getvar);
  def ("setvar", pytave::setvar);
  def ("isvar", pytave::isvar);
  def ("delvar", pytave::delvar);
  def ("push_scope", pytave::push_scope);
  def ("pop_scope", pytave::pop_scope);
  def ("atexit", pytave::atexit);
  def ("get_exceptions", pytave::get_exceptions);

  register_exception_translator<pytave::pytave_exception> (
    pytave::pytave_exception::translate_exception);

  register_exception_translator<pytave::octave_error_exception> (
    pytave::octave_error_exception::translate_exception);

  register_exception_translator<pytave::octave_parse_exception> (
    pytave::octave_parse_exception::translate_exception);

  register_exception_translator<pytave::object_convert_exception> (
    pytave::object_convert_exception::translate_exception);

  register_exception_translator<pytave::value_convert_exception> (
    pytave::value_convert_exception::translate_exception);

  register_exception_translator<pytave::variable_name_exception> (
    pytave::variable_name_exception::translate_exception);
}
