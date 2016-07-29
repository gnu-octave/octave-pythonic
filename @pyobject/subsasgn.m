## Copyright (C) 2016 Colin B. Macdonald
##
## This file is part of Pytave.
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
## @defop  Method   @@pyobject subsasgn (@var{x}, @var{idx}, @var{rhs})
## @defopx Operator @@pyobject {@var{x}.@var{property} =} {@var{rhs}}
## @defopx Operator @@pyobject {@var{x}@{@var{i}@} =} {@var{rhs}}
## @defopx Operator @@pyobject {@var{x}@{@var{i}, @var{j}, @dots{}@} =} {@var{rhs}}
## @defopx Operator @@pyobject {@var{x}@{@var{a}@} =} {@var{rhs}}
## Indexed assignment to Python objects.
##
## @seealso{@@pyobject/subsref}
## @end defop

function r = subsasgn(x, idx, rhs)

  if (nargin != 3)
    print_usage ();
  endif

  ## If rhs is a pyobject but x is not, dispatch to the builtin subsasgn
  if (! isa (x, "pyobject"))
    r = builtin ("subsasgn", x, idx, rhs);
    return;
  endif

  switch idx.type
    case "."
      assert (ischar (idx.subs))
      pycall ("setattr", x, idx.subs, rhs);
      r = x;

    case "{}"
      ## XXX: doesn't support slices or anything like that yet

      ## Subtract one from index: do this for lists, numpy arrays, etc
      pyexec ("import collections")
      pyexec ("import numpy")
      x_is_list = pycall (pyeval (
        "lambda x: isinstance(x, (collections.Sequence, numpy.ndarray))"),
        x);
      for i = 1:length (idx.subs)
        j = idx.subs{i};
        if (x_is_list && isindex (j) && isnumeric (j))
          idx.subs{i} = cast (j, class (sizemax ())) - 1;
        endif
      endfor

      if (isscalar (idx.subs))
        ind = idx.subs{1};
      else
        error ("not implemented, waiting on #26, #27")
      endif

      xsi = pycall ("getattr", x, "__setitem__");   # x.__setitem__
      pycall (xsi, ind, rhs);
      r = x;

    otherwise
      idx
      rhs
      error ("@pyobject/subsasgn: not implemented")
  endswitch
endfunction


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

%!test
%! % dict assignment, adding new keys
%! pyexec ("d = dict()")
%! d = pyobject.fromPythonVarName ("d");
%! d{"a"} = 3;
%! d{"b"} = 4;
%! assert (d{"a"}, 3)
%! assert (d{"b"}, 4)

%!test
%! % dict assignment, update existing key
%! pyexec ("d = {'a':1}")
%! d = pyobject.fromPythonVarName ("d");
%! d{"a"} = 3;
%! assert (d{"a"}, 3)

%!test
%! % dict assignment, other keys (e.g., Issue #10).
%! pyexec ("d = dict()")
%! d = pyobject.fromPythonVarName ("d");
%! d{"5"} = 10;
%! d{5.5} = 11;
%! d{5} = 12;
%! assert (d{"5"}, 10)
%! assert (d{5.5}, 11)
%! assert (d{5}, 12)
