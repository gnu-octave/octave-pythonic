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

#if defined (HAVE_CONFIG_H)
#  include <config.h>
#endif

#include <Python.h>

#include "oct-py-init.h"

namespace pytave
{

#if PY_VERSION_HEX >= 0x03000000
  static wchar_t sys_argv0[] {L'\0'};
  static wchar_t *sys_argv[] {sys_argv0, nullptr};
#else
  static char sys_argv0[] {'\0'};
  static char *sys_argv[] {sys_argv0, nullptr};
#endif

  void
  py_init ()
  {
    bool is_initialized = Py_IsInitialized ();

    Py_Initialize ();

    if (! is_initialized)
      PySys_SetArgvEx (1, sys_argv, 0);
  }

}
