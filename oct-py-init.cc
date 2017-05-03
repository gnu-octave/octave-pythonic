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

namespace pytave
{

#if PY_VERSION_HEX >= 0x03000000
  wchar_t *sys_argv[] { L"", nullptr };
#else
  char *sys_argv[] { "", nullptr };
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
