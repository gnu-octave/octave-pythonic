## Copyright (C) 2016 Colin B. Macdonald
## Copyright (C) 2016-2019 Mike Miller
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
## @defmethod @@pyobject cell (@var{x})
## Convert a Python list or other object implementing the Sequence protocol
## to a cell array.
##
## For example, by default Python lists are not automatically
## converted into native Octave objects:
## @example
## @group
## L = pyeval ("[10.0, 20.0, 'hello']")
##   @result{} L = [Python object of type list]
##
##       [10.0, 20.0, 'hello']
## @end group
## @end example
##
## However, we can convert the list to a cell array:
## @example
## @group
## @c FIXME: display of cell array in flux: https://savannah.gnu.org/bugs/?50756
## @c doctest: +XFAIL
## C = cell (L)
##   @result{} C =
##     @{
##       [1,1] =  10
##       [1,2] =  20
##       [1,3] = [Python object of type str]
##
##                 hello
##     @}
## @end group
## @end example
##
## The conversion is not recursive, in the following sense:
## @example
## @group
## @c FIXME: display of cell array in flux: https://savannah.gnu.org/bugs/?50756
## @c doctest: +XFAIL
## L = pyeval ("[10.0, 20.0, [33.0, 44.0], 50.0]");
## C = cell (L)
##   @result{} C =
##     @{
##       [1,1] =  10
##       [1,2] =  20
##       [1,3] = [Python object of type list]
##
##                 [33.0, 44.0]
##
##       [1,4] =  50
##     @}
## @end group
## @end example
##
## @seealso{cell2mat}
## @end defmethod


function c = cell (x)
  ## FIXME: when subsref returns the right number of output args, this can
  ##        simply be "c = {x{:}}"
  n = length (x);
  c = cell (1, n);
  if (n > 0)
    [c{:}] = subsref (x, struct ("type", "{}", "subs", {{":"}}));
  endif
endfunction


%!assert (cell (pyeval ("[]")), cell (1, 0))
%!assert (cell (pyeval ("[1.]")), {1})
%!assert (cell (pyeval ("[1., 2., 3.]")), {1, 2, 3})
%!assert (cell (pyeval ("(1., 2., 3.)")), {1, 2, 3})
%!assert (cellfun (@char, cell (pyobject ("asdf")), "uniformoutput", false), {"a", "s", "d", "f"})

%!test
%! c = cell (pyeval ("range(10)"));
%! c = cellfun (@(x) eval ("double (x)"), c, "uniformoutput", false);
%! assert (c, num2cell (0:9))

%!error cell (pyobject ())
%!error cell (pyeval ("None"))
%!error cell (pyobject (1))
