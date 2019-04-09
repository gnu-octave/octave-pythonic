/*

SPDX-License-Identifier: GPL-3.0-or-later

Copyright (C) 2017 Mike Miller

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

#if ! defined (pytave_oct_py_error_h)
#define pytave_oct_py_error_h 1

#include <string>

#if defined (__GNUC__)
#  define PYTAVE_ATTR_NORETURN __attribute__((__noreturn__))
#else
#  define PYTAVE_ATTR_NORETURN
#endif

namespace pytave
{

  void
  error_conversion_invalid_python_object (const std::string& to)
  PYTAVE_ATTR_NORETURN;

  void
  error_conversion_mismatch_python_type (const std::string& to,
                                         const std::string& must)
  PYTAVE_ATTR_NORETURN;

  void
  error_python_exception ()
  PYTAVE_ATTR_NORETURN;

}

#undef PYTAVE_ATTR_NORETURN

#endif
