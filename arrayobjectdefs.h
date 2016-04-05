/*

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

/* If your extension does not reside in a single file, there is an
 * additional step that is necessary. Be sure to define the symbol
 * PY_ARRAY_UNIQUE_SYMBOL to some name (the same name in all the files
 * comprising the extension), upstream from the include of
 * arrayobject.h. Typically this would be in some header file that is
 * included before arrayobject.h.
 */
#ifndef PYTAVE_DO_DECLARE_SYMBOL
#define NO_IMPORT_ARRAY
#endif
#define PY_ARRAY_UNIQUE_SYMBOL pytave_array_symbol
#include <Python.h>
#include <numpy/npy_no_deprecated_api.h>
#include <numpy/arrayobject.h>
