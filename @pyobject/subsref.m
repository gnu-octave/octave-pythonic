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


function varargout = subsref (x, idx)

  t = idx(1);
  switch t.type
    case "()"
      r = pycall (x, t.subs{:});

    case "."
      assert (ischar (t.subs))
      r = pycall ("getattr", x, t.subs);

    case "{}"
      subsstrs = {};
      for j = 1:length (t.subs)
        thissub = t.subs{j};
        if (ischar (thissub) && strcmp (thissub, ":"))
          subsstrs{j} = ":";
        elseif (ischar (thissub))
          subsstrs{j} = ["'" thissub "'"];
        elseif (isnumeric (thissub) && isscalar (thissub))
          ## note: python indexed from 0
          subsstrs{j} = num2str (thissub - 1);
        else
          thissub
          error ("@pyobject/subsref: subs not supported")
        endif
      endfor
      s = ["[" strjoin(subsstrs, ", ") "]"];
      ## XXX: can we use .__getitem__ here?
      r = pyeval (sprintf ("__InOct__['%s']%s", x.id, s));

    otherwise
      t
      error ("@pyobject/subsref: not implemented")
  endswitch

  ## deal with additional indexing (might be recursive)
  if (length (idx) > 1)
    r = subsref (r, idx(2:end));
  endif

  ## unpack results, ensure "ans" works (see also pycall)
  isNone = pyeval ("lambda x: x is None");
  if (nargout > 0 || ! pycall (isNone, r))
    varargout{1} = r;
  endif
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

%!test
%! % method call with args
%! s = pyeval ("set({1, 2})");
%! s.add (42)
%! assert (length (s) == 3)

%!test
%! % get a callable
%! s = pyeval ("set({1, 2})");
%! sa = s.add;
%! assert (isa (sa, "pyobject"))
%! % and then call it
%! sa (42)
%! assert (length (s) == 3)

%!test
%! % callable can return something
%! s = pyeval ("set({1, 2})");
%! v = s.pop ();
%! assert (length (s) == 1)
%! assert (v == 1 || v == 2)

%!test
%! % chain
%! pyexec ("import sys")
%! s = pyeval ("set({sys})");
%! ver = s.pop ().version;
%! assert (ischar (ver))

%!test
%! % don't set "ans" if no return value
%! s = pyeval ("set({1, 2})");
%! sa = s.add;
%! clear ans
%! sa (42)
%! assert (! exist ("ans", "var"))

%!test
%! % *do* set "ans" if return value
%! s = pyeval ("set({1, 2})");
%! clear ans
%! s.pop ();
%! assert (exist ("ans", "var"))
%! assert (length (s) == 1)
