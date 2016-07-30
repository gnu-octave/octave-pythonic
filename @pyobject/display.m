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
## @defmethod @@pyobject display (@var{x})
## Custom display for pyobjects.
##
## Example:
## @example
## @group
## pyexec ("import sys")
## sysmodule = pyeval ("sys")
##   @result{} sysmodule = [pyobject ...]
##
##       <module 'sys' (built-in)>
##
## @end group
## @end example
##
## @seealso{@@pyobject/char, @@pyobject/disp}
## @end defmethod


function display (x)

  loose = ! __compactformat__ ();

  printf ("%s = [pyobject %s]\n", inputname (1), getid (x));
  s = char (x);
  s = make_indented (s);
  if (loose), printf ("\n"); endif
  disp (s)
  if (loose), printf ("\n"); endif

endfunction


function s = make_indented (s, n)
  if (nargin == 1)
    n = 2;
  endif
  pad = char (double (" ") * ones (1,n));
  s = strrep (s, "\n", ["\n" pad]);
  s = [pad s];  # first line
endfunction
