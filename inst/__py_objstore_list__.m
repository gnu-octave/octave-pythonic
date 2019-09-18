## Copyright (C) 2019 Colin B. Macdonald
## SPDX-License-Identifier: GPL-3.0-or-later
##
## This file is part of Octave Pythonic.
##
## Octave Pythonic is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## Octave Pythonic is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Octave Pythonic; see the file COPYING.  If not, see
## <https://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @deftypefn {} {} __py_objstore_list__ ()
## Print or return all objects and their ref counts in the object store.
##
## This is a private internal function not intended for direct use.\n\
## @end deftypefn

function retval = __py_objstore_list__ ()

  x = __py_objstore_list_backend__ ();

  if (nargout != 0)
    retval = x;
    return
  endif

  sz = numel (x);
  use_unicode = ! (ispc () && (! isunix ()));
  cols = terminal_size ()(2);
  if (sz < 1)
    disp ("Python object store is empty")
  else
    disp ("Contents of the Python object store:\n")
    disp ("  key            count  type          string snippet")
    if (use_unicode)
      disp ("  ───            ─────  ────          ──────────────")
    else
      disp ("  ---            -----  ----          --------------")
    endif
  endif

  TypeLen = 12;
  SnipLen = max (42, cols - (80 - 42)) - 2;

  for i=1:sz
    row = x{i};
    snip = undo_string_escapes (row{4});
    if (length (snip) > SnipLen)
      if (use_unicode)
        snip = [strtrunc(snip, SnipLen - 1) "…"];
      else
        snip = [strtrunc(snip, SnipLen - 3) "..."];
      endif
    endif
    type = row{3};
    if (length (type) > TypeLen)
      if (use_unicode)
        type = [strtrunc(type, TypeLen - 1) "…"];
      else
        type = [strtrunc(type, TypeLen - 3) "..."];
      endif
    endif
    printf ("  %#.12x %5d  %-12s  %s\n", row{1}, row{2}, type, snip)
  endfor
  if (sz >= 1)
    disp ("")
  endif

endfunction
