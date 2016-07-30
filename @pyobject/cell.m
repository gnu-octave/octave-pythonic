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
## @defmethod @@pyobject cell (@var{x})
## Convert an iterable Python object to a cell array.
##
## For example, by default Python lists are not automatically
## converted into native Octave objects:
## @example
## @group
## L = pyeval ("[10, 20, 'hello']")
##   @result{} L = [pyobject ...]
##
##       [10, 20, 'hello']
## @end group
## @end example
##
## However, we can convert the list to a cell array:
## @example
## @group
## C = cell (L)
##   @result{} C =
##     @{
##       [1,1] =  10
##       [1,2] =  20
##       [1,3] = hello
##     @}
## @end group
## @end example
##
## The conversion is not recursive, in the following sense:
## @example
## @group
## L = pyeval ("[10, 20, [33, 44], 50]");
## C = cell (L)
##   @result{} C =
##     @{
##       [1,1] =  10
##       [1,2] =  20
##        ...  = [pyobject ...]
##
##                 [33, 44]
##
##       [1,4] =  50
##     @}
## @end group
## @end example
##
## @seealso{cell2mat}
## @end defmethod


function c = cell (L)
  ## XXX: better implementation available possible in C++?

  c = {};
  #c = cell (size (L));  # scary, b/c "size" calls "cell"

  for i = 1:length (L)  # not numel
    c{i} = subsref (L, struct ("type", "{}", "subs", {{i}}));
  endfor
endfunction


%!test
%! L = pyeval ("(1, 2, 3)");
%! C = cell (L);
%! assert (iscell (C))
%! assert (C, {1, 2, 3})
