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

%!test
%! if (double (py.sys.hexversion) >= 0x03000000)
%!   assert (isobject (py.int (0)))
%! else
%!   assert (py.int (0), int64 (0))
%! endif

%!test
%! if (double (py.sys.hexversion) < 0x03000000)
%!   assert (py.int (2147483647), int64 (2147483647))
%! endif

%!test
%! if (double (py.sys.hexversion) < 0x03000000)
%!   assert (isobject (py.long (0)))
%! endif

%!test
%! if (double (py.sys.hexversion) >= 0x03000000)
%!   assert (isobject (py.int (2^100)))
%! else
%!   assert (isobject (py.long (2^100)))
%! endif

## Cannot use '@' to make a handle to a Python function
%!xtest
%! fn = @py.math.abs;
%! assert (fn (-3), 3)
