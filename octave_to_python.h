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

/*! \file octave_to_python.h
    \brief Conversion of data values from GNU Octave to Python

    The functions in this file define the translation of GNU Octave
    values to Python objects using the Boost.Python library and
    the Octave/C++ API.

    This project is currently derived from an earlier project called
    Pytave that allowed Python to call Octave functions on an
    embedded Octave interpreter. The bulk of the project is in the
    code to convert between Octave and Python data types, so most of
    that is reusable. As a side goal, we may continue to maintain the
    Python wrapper around Octave and incorporate that into Octave as
    well, so that Octave can provide its own native Python module.
*/


#if ! defined (pytave_octave_to_python_h)
#define pytave_octave_to_python_h

#include <boost/python.hpp>
#include <ov.h>

//! Contains the functions used for conversion.
namespace pytave
{
  //! Conversion from any Octave value object to a Python object.
  /*!
    \param py_object a reference to boost::python::object to store the result of the conversion.
    \param oct_value a constant reference to a octave_value that contains the object to be converted.
    \see pyobj_to_octvalue
  */
  void octvalue_to_pyobj (boost::python::object& py_object,
                          const octave_value& oct_value);

  //! Octave value list objects are converted to Python tuples.
  /*!
    \param py_tuple a reference to boost::python::tuple to store
    the result of the conversion.
    \param oct_list a constant reference to a octave_value_list
    that contains the tuple to be converted.
    \see pytuple_to_octlist
  */
  void octlist_to_pytuple (boost::python::tuple& py_tuple,
                           const octave_value_list& oct_list);
}

#endif
