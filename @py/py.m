## Copyright (C) 2016 Mike Miller
##
## This file is part of Pytave.
##
## Pytave is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or (at
## your option) any later version.
##
## Pytave is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Pytave; see the file COPYING.  If not, see
## <http://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @deftypefn  {} {} py
## @deftypefnx {} {} py.@var{pyname}
## Get the value of a Python object or call a Python function.
## @end deftypefn

function p = py ()
  p = class (struct (), "py");
endfunction

%!assert (py.math.sqrt (2), sqrt (2))
%!assert (ischar (py.sys.version))
