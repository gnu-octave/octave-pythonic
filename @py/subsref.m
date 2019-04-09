## Copyright (C) 2016 Mike Miller
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

  if (type == "." && ((numel (idx) == 1) || (idx(2).type != ".")))
    try
      y = pyeval (subs);
    catch
      y = pycall ("__import__", subs);
    end_try_catch
  else
    y = pycall ("__import__", subs);
  endif

  if (numel (idx) > 1)
    y = subsref (y, idx(2:end));
  endif

  ## If the *last* indexing operation is ".name", and the object returned
  ## is a Python callable, then call it with no arguments to be compatible
  ## with how Octave functions are evaluated.
  if (idx(end).type == ".")
    if (isa (y, "py.collections.Callable"))
      y = pycall (y);
    endif
  endif

  if (nargout > 0 || ! __py_is_none__ (y))
    varargout{1} = y;
  endif

endfunction
