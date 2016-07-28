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
## @deftypefn {} {} subsref (@var{x}, @var{idx})
## Implements Python name lookup via dot indexing.
## @enddeftypefn

function varargout = subsref (x, idx)

  if (nargin != 2)
    print_usage ();
  endif

  if (! isa (x, "py"))
    error ("py: invalid call to subsref");
  endif

  if (isempty (idx) || ! isstruct (idx))
    error ("py: invalid call to subsref without indices");
  endif

  type = idx(1).type;
  subs = idx(1).subs;

  if (type != ".")
    error ("py: invalid indexing type");
  endif

  y = pycall ("__import__", subs);

  if (numel (idx) > 1)
    y = subsref (y, idx(2:end));
  endif

  is_none = pyeval ("lambda x: x is None");
  if (nargout > 0 || ! pycall (is_none, y))
    varargout{1} = y;
  endif

endfunction
