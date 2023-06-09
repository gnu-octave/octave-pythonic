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
## @documentencoding UTF-8
## @defmethod @@pyobject fieldnames (@var{x})
## List the properties of a Python object.
##
## Returns a cell array of strings containing the names of the properties
## of @var{x}.
##
## Example:
## @example
## @group
## pyexec ("import sys")
## sys = pyeval ("sys");
## fieldnames (sys)
##   @result{} ans =
##     @{
##       [1,1] = ...
##       [2,1] = ...
##        ...  = path
##        ...  = version
##        ...
##     @}
## @end group
## @end example
##
## @seealso{fieldnames, @@pyobject/methods}
## @end defmethod


function names = fieldnames (x)

  cmd = pyeval (["lambda x: [a for a in dir(x)" ...
                 " if not callable(getattr(x, a))" ...
                 " and not isinstance(getattr(x, a), __import__('types').ModuleType)" ...
                 " and not a.startswith('_')]"]);

  names_obj = pycall (cmd, x);
  names = cellfun (@char, cell (names_obj), "uniformoutput", false);
  names = names(:);

endfunction


%!test
%! sys = pycall ("__import__", "sys");
%! lst = fieldnames (sys);
%! assert (iscell (lst))
%! assert (length (lst) >= 32)
%! assert (any (strcmp (lst, "path")))
%! assert (any (strcmp (lst, "prefix")))
%! assert (any (strcmp (lst, "stderr")))
%! assert (any (strcmp (lst, "stdin")))
%! assert (any (strcmp (lst, "stdout")))
%! assert (any (strcmp (lst, "version")))

%!test
%! lst = fieldnames (pyeval ("__builtins__"));
%! assert (any (strcmp (lst, "False")))
%! assert (any (strcmp (lst, "None")))
%! assert (any (strcmp (lst, "True")))

%!assert (fieldnames (pyeval ("object()")), cell (0, 1))
%!assert (fieldnames (pyeval ("{}")), cell (0, 1))
%!assert (ismember ("denominator", fieldnames (pycall ("fractions.Fraction"))))
