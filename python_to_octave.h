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

/*! \file python_to_octave.h
    \brief Conversion of data values from Python to GNU Octave

    The functions in this file define the translation of Python
    objects to GNU Octave values using the Boost.Python library and
    the Octave/C++ API.

    Features and capabilities of Octave's Python interface include:

    - Import and call Python modules and functions from the Octave interpreter

    - Automatically convert basic Octave and Python types seamlessly between the two environments

    - \todo Be able to handle arbitrary unknown Python objects (print their repr, store in a variable, pass back in to a Python function)

    -  \todo Store references to Python functions (and other "callables") and be able to call them as if they were function handles

*/

#if ! defined (pytave_python_to_octave_h)
#define pytave_python_to_octave_h

#include <boost/python.hpp>
#include <ov.h>

namespace pytave
{

  //! Conversion from any Python object to an Octave value object.
  /*!
    \param oct_value a reference to octave_value to store the result of the conversion.
    \param py_object a constant reference to a boost::python::object that contains the object to be converted.
    \see octvalue_to_pyobj
  */
  void pyobj_to_octvalue (octave_value& oct_value,
                          const boost::python::object& py_object);
}

#endif
