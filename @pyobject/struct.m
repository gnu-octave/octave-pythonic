## Copyright (C) 2016 Mike Miller
##
## This file is part of Pytave
##
## Pytave is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published
## by the Free Software Foundation; either version 3 of the License,
## or (at your option) any later version.
##
## This software is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty
## of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
## the GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public
## License along with this software; see the file COPYING.
## If not, see <http://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @documentencoding UTF-8
## @defmethod @@pyobject struct (@var{x})
## Convert a Python object to an Octave struct containing copies of its
## public properties.
##
## Example:
## @example
## @group
## os = pycall ("__import__", "os");
## struct (os)
##   @result{} scalar structure containing the fields:
##       ...
##       ...
##       environ = ...
##       name = ...
##       ...
## @end group
## @end example
##
## @seealso{struct, @@pyobject/fieldnames}
## @end defmethod


function retval = struct (x)

  retval = struct ();
  names = fieldnames (x);
  for i = 1:numel (names)
    retval = setfield (retval, names{i}, pycall ("getattr", x, names{i}));
  endfor

endfunction


%!test
%! os = pycall ("__import__", "os");
%! rv = struct (os);
%! assert (isstruct (rv))
%! assert (numfields (rv) >= 10)
%! assert (fieldnames (rv), fieldnames (os))
%! assert (isfield (rv, "curdir"))
%! assert (isfield (rv, "devnull"))
%! assert (isfield (rv, "environ"))
%! assert (isfield (rv, "name"))
%! assert (isfield (rv, "pathsep"))
%! assert (isfield (rv, "sep"))

%!test
%! rv = struct (pyeval ("__builtins__"));
%! assert (rv.False, false)
%! assert (rv.True, true)
%! assert (isequal (rv.None, pyeval ("None")))
