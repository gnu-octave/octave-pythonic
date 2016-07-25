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
## @defop  Method   @@pyobject subsasgn (@var{x}, @var{idx}, @var{rhs})
## @defopx Operator @@pyobject {@var{x}.@var{property} =} {@var{rhs}}
## @defopx Operator @@pyobject {@var{x}@{@var{i}@} =} {@var{rhs}}
## Indexed assignment to Python objects.
##
## @seealso{@@pyobject/subsref}
## @end defop

function r = subsasgn(x, idx, rhs)

  switch idx.type
    case "."
      assert (ischar (idx.subs))
      pycall ("setattr", x, idx.subs, rhs);
      r = x;

    case "{}"
      ## XXX: doesn't support slices or anything like that yet
      assert (length (idx.subs) == 1)
      assert (isnumeric (idx.subs{1}))
      ## x.getattr, but for https://github.com/cbm755/octsympy/issues/17
      xsi = pycall ("getattr", x, "__setitem__");
      ## 1-based indexing, https://bitbucket.org/mtmiller/pytave/issues/8
      i = int32 (idx.subs{:}) - 1;
      pycall (xsi, i, rhs);
      r = x;

    otherwise
      idx
      rhs
      error ("@pyobject/subsasgn: not implemented")
  end
end


%!test
%! pyexec ("class MyClass: a = 1")
%! t = pyeval ("MyClass()");
%! t.b = 6;
%! assert (t.b, 6)

%!test
%! % list indexing
%! pyexec ("L = [10, 20]")
%! L = pyobject.fromPythonVarName ("L");
%! L{2} = "Octave";
%! assert (length (L) == 2)
%! assert (L{1}, 10)
%! assert (L{2}, "Octave")
