## Copyright (C) 2019 Colin B. Macdonald
## Copyright (C) 2019 Mike Miller
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
## @deftypefn  {} {} __py_objstore_disp__ ()
## @deftypefnx {} {@var{list} =} __py_objstore_disp__ ()
## Print or return all Python objects and their ref counts in the object store.
##
## This is a private internal function not intended for direct use.
## @end deftypefn

function retval = __py_objstore_disp__ ()

  x = __py_objstore_list__ ();

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

  for i = 1:sz
    snip = undo_string_escapes (x(i).value);
    if (length (snip) > SnipLen)
      if (use_unicode)
        snip = [strtrunc(snip, SnipLen - 1) "…"];
      else
        snip = [strtrunc(snip, SnipLen - 3) "..."];
      endif
    endif
    type = x(i).type;
    if (length (type) > TypeLen)
      if (use_unicode)
        type = [strtrunc(type, TypeLen - 1) "…"];
      else
        type = [strtrunc(type, TypeLen - 3) "..."];
      endif
    endif
    printf ("  %#.12x %5d  %-12s  %s\n", x(i).key, x(i).count, type, snip)
  endfor
  if (sz >= 1)
    disp ("")
  endif

endfunction

## Mark this file as fully tested.
%!assert (1)
