## Copyright (C) 2016 Colin B. Macdonald
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
## @defmethod @@pyobject methods (@var{x})
## List the properties/callables of a Python object.
##
## Returns a cell array of strings, the names of the properties
## and ``callables'' of @var{x}.
##
## Example:
## @example
## @group
## pyexec ("import sys")
## sys = pyeval ("sys");
## methods (sys)
##   @result{} ans =
##     @{
##       [1,1] = ...
##       [1,2] = ...
##        ...  = path
##        ...  = version
##        ...
##     @}
## @end group
## @end example
##
## Note that if you instead want the methods implemented by
## the Octave class @code{@@pyobject}, use can always do:
## @example
## @group
## methods pyobject
##   @print{} Methods for class pyobject:
##   @print{} display  ...  subsref
## @comment this doctest may need updating as we add methods
## @end group
## @end example
##
## @seealso{methods}
## @end defmethod


function L = methods (x)
  # filter the output of `dir(x)`
  # (to get properties only:
  # [a for a in dir(x) if not callable(getattr(x, a)) and not a.startswith('__')]
  cmd = sprintf ("[a for a in dir(__InOct__['%s']) if not a.startswith('__')]",
                 getid (x));
  # TODO: may need to convert from Python list to Octave list
  L = pyeval (cmd);
endfunction


%!test
%! pyexec ("import sys")
%! sys = pyeval ("sys");
%! L = methods (sys);
%! % sys has lots of methods
%! assert (length (L) >= 32)
%! % version is one of them
%! assert (any (strcmp (L, "version")))

%!test
%! pyexec ("import sys")
%! L = methods (pyeval ("sys"));
%! assert (iscell (L))
