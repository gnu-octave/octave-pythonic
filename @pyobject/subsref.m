## Copyright (C) 2016 Colin B. Macdonald
##
## This file is part of PyTave.
##
## OctSymPy is free software; you can redistribute it and/or modify
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
## @defop  Method   @@pyobject subsref (@var{x}, @var{idx})
## @defopx Operator @@pyobject {@var{x}.@var{property}} {}
## @defopx Operator @@pyobject {@var{x}.@var{method}(@var{a}, @dots{})} {}
## @defopx Operator @@pyobject {@var{x}@{@var{i}@}} {}
## @defopx Operator @@pyobject {@var{x}@{@var{i}, @var{j}, @dots{}@}} {}
## @defopx Operator @@pyobject {@var{x}(@var{a})} {}
## @defopx Operator @@pyobject {@var{x}(@var{a}, @var{b}, @dots{})} {}
## Call methods and access properties of a Python object.
##
##
## @seealso{@@pyobject/subsasgn}
## @end defop


function r = subsref (x, idx)
  s = "";
  for i = 1:length (idx)
    t = idx(i);
    switch t.type
      case "()"
        if (! isempty (t.subs))
          t
          error ("not implemented: function calls with arguments")
        endif
        s = sprintf ("%s()", s);
      case "."
        assert (ischar (t.subs))
        s = sprintf ("%s.%s", s, t.subs);
      case "{}"
        subsstrs = {};
        for j = 1:length (t.subs)
	  thissub = t.subs{j};
          if (ischar (thissub) && strcmp (thissub, ":"))
            subsstrs{j} = ":";
          elseif (ischar (thissub))
            subsstrs{j} = ["'" thissub "'"];
          elseif (isnumeric (thissub) && isscalar (thissub))
	    % note: python indexed from 0
            subsstrs{j} = num2str (thissub - 1);
          else
            thissub
            error ("@pyobject/subsref: subs not supported")
          endif
        endfor
        s = [s "[" strjoin(subsstrs, ", ") "]"];
      otherwise
        t
        error ("@pyobject/subsref: not implemented")
    endswitch
  endfor
  r = pyeval (sprintf ("__InOct__['%s']%s", x.id, s));
endfunction


%!test
%! % list indexing
%! pyexec ("L = [10, 20]")
%! L = pyobject.fromPythonVarName ("L");
%! assert (L{1}, 10)
%! assert (L{2}, 20)

%!test
%! % list indexing
%! pyexec ("L = [10, 20, [30, 40]]")
%! L = pyobject.fromPythonVarName ("L");
%! L2 = L{:};
%! assert (L2{1}, 10)
%! assert (L2{2}, 20)
%! assert (L2{3}{1}, 30)
%! assert (L2{3}{2}, 40)

%!test
%! % list indexing, nested list
%! pyexec ("L = [1, 2, [10, 11, 12]]")
%! L = pyobject.fromPythonVarName ("L");
%! assert (L{2}, 2)
%! assert (L{3}{1}, 10)
%! assert (L{3}{3}, 12)

%!test
%! % 2D array indexing
%! pyexec ("import numpy")
%! pyexec ("A = numpy.array([[1, 2], [3, 4]])")
%! A = pyobject.fromPythonVarName ("A");
%! assert (A{1, 1}, 1)
%! assert (A{2, 1}, 3)
%! assert (A{1, 2}, 2)

%!test
%! % dict: str key access
%! pyexec ("d = {'one':1, 5:5, 6:6}")
%! d = pyobject.fromPythonVarName ("d");
%! assert (d{"one"}, 1)

%!test
%! % dict: integer key access
%! pyexec ("d = {5:42, 6:42}")
%! d = pyobject.fromPythonVarName ("d");
%! assert (d{6}, 42)

%!xtest
%! % dict: integer key should not subtract one (FIXME: Issue #10)
%! pyexec ("d = {5:40, 6:42}")
%! d = pyobject.fromPythonVarName ("d");
%! assert (d{6}, 42)
