/*

Copyright (C) 2017 Mike Miller

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

#include "oct-py-init.h"

// FIXME: the following are only needed for Boost.Python library and NumPy
// library initialization
#define PYTAVE_DO_DECLARE_SYMBOL
#include <boost/python.hpp>
#include "arrayobjectdefs.h"

namespace pytave
{

  void
  py_init ()
  {
    Py_Initialize ();

    // FIXME: these are only needed for Boost.Python implicit conversion
    // of Octave arrays to NumPy arrays
    boost::python::numeric::array::set_module_and_type ("numpy", "ndarray");
    _import_array ();
  }

}