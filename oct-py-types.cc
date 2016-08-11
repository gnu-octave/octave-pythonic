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

#if defined (HAVE_CONFIG_H)
#  include <config.h>
#endif

#include <octave/oct-map.h>
#include <octave/quit.h>

#include "oct-py-types.h"

// FIXME: only here to bootstrap nested conversions needed in this file
#include "octave_to_python.h"

namespace pytave
{

inline PyObject *
wrap_octvalue_to_pyobj (const octave_value& value)
{
  boost::python::object obj;
  octvalue_to_pyobj (obj, value);
  PyObject *ptr = obj.ptr ();
  Py_INCREF (ptr);
  return ptr;
}

PyObject *
make_py_dict (const octave_scalar_map& map)
{
  PyObject *dict = PyDict_New ();
  if (! dict)
    octave_throw_bad_alloc ();

  for (auto p = map.begin (); p != map.end (); ++p)
    {
      PyObject *key = wrap_octvalue_to_pyobj (map.key (p));
      PyObject *item = wrap_octvalue_to_pyobj (map.contents (p));

      if (PyDict_SetItem (dict, key, item) < 0)
        throw boost::python::error_already_set ();
    }

  return dict;
}

}
