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
      ## Subtract one from index: do this for lists, numpy arrays, etc
      pyexec ("import collections")
      pyexec ("import numpy")
      x_is_list = pycall (pyeval (
        "lambda x: isinstance(x, (collections.Sequence, numpy.ndarray))"),
        x);
      for i = 1:length(t.subs)
        j = t.subs{i};
        if (isindex (j) && isnumeric (j) && x_is_list)
          t.subs{i} = cast (j, class (sizemax ())) - 1;
        endif
      endfor

      if (ischar (t.subs{1}) && strcmp (t.subs{1}, ":"))
        is_map = pyeval ("lambda x: isinstance(x, collections.Mapping)");
        if (pycall (is_map, x))
          ind = ":";
        else
          ind = int32 ([1:length(x)] - 1);
        endif
      elseif (isscalar (t.subs))
        ind = t.subs{1};
      else
        ## XXX: after #26, #27, I think its just:
        #ind = pycall ("tuple", t.subs);
        pyexec (["global _temp\n" ...
                 "def pystoretemp(x):\n" ...
                 "    global _temp\n" ...
                 "    _temp = x"]);
        pycall ("pystoretemp", t.subs);
        pyexec ("_temp = tuple(_temp[0])");
        ind = pyobject.fromPythonVarName ("_temp");
      endif

      gi = pycall ("getattr", x, "__getitem__");   # x.__getitem__
      if (isnumeric (ind) && length (ind) > 1)
        r = {};
        for k = 1:length (ind)
          r(end+1) = pycall (gi, ind(k));
        endfor
      else
        r = pycall (gi, ind);
      endif

    otherwise
      t
      error ("@pyobject/subsref: not implemented")
  endswitch

  ## deal with additional indexing (might be recursive)
  if (length (idx) > 1)
    r = subsref (r, idx(2:end));
  endif

  ## unpack results, ensure "ans" works (see also pycall)
  is_none = pyeval ("lambda x: x is None");
  if (nargout == 0 && ! pycall (is_none, r))
    varargout{1} = r;
  elseif (nargout == 1)
    varargout{1} = r;
  elseif (nargout >= 2)
    assert (length (r) == nargout, ...
            "pyobject/subsref: number of outputs must match")
    [varargout{1:nargout}] = subsref (r, struct ("type", "{}", "subs", {{1:nargout}}));
  endif
endfunction


%!test
%! % list indexing
%! L = pyeval ("[10, 20]");
%! assert (L{1}, 10)
%! assert (L{2}, 20)

%!test
%! % list indexing, slice
%! L = pyeval ("[10, 20, [30, 40]]");
%! L2 = L{:};
%! assert (L2{1}, 10)
%! assert (L2{2}, 20)
%! assert (L2{3}{1}, 30)
%! assert (L2{3}{2}, 40)

%!test
%! % list indexing, nested list
%! L = pyeval ("[1, 2, [10, 11, 12]]");
%! assert (L{2}, 2)
%! assert (L{3}{1}, 10)
%! assert (L{3}{3}, 12)

%!test
%! % list indexing, assign to vars
%! L = pyeval ("[1, 2, 'Octave']");
%! [a, b, c] = L{:};
%! assert (a, 1)
%! assert (b, 2)
%! assert (c, "Octave")

%!test
%! % 2D array indexing
%! pyexec ("import numpy")
%! A = pyobject (pyeval ("numpy.array([[1, 2], [3, 4]])"));
%! assert (A{1, 1}, 1)
%! assert (A{2, 1}, 3)
%! assert (A{1, 2}, 2)

%!test
%! % dict: str key access
%! d = pyeval ("{'one':1, 5:5, 6:6}");
%! assert (d{"one"}, 1)

%!test
%! % dict: integer key access
%! d = pyeval ("{5:42, 6:42}");
%! assert (d{6}, 42)

%!test
%! % dict: integer key should not subtract one
%! d = pyeval ("{5:40, 6:42}");
%! assert (d{6}, 42)

%!test
%! % dict: floating point keys should work
%! d = pyeval ("{5.5:'ok'}");
%! assert (d{5.5}, "ok")

%!test
%! % dict: make sure key ":" doesn't break anything
%! d = pyeval ("{'a':1, ':':2}");
%! assert (d{'a'}, 1)
%! assert (d{':'}, 2)

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

%!test
%! % multiple return values: can get all of them
%! f = pyeval ("lambda: (1, 2, 3)");
%! a = f ();
%! assert (length (a) == 3)

%!test
%! % multiple return values: separate them
%! f = pyeval ("lambda: (1, 2, 3)");
%! [a, b, c] = f ();
%! assert (a, 1)
%! assert (b, 2)
%! assert (c, 3)

%!test
%! % multiple return values: set ans
%! f = pyeval ("lambda: (1, 2, 3)");
%! f ();
%! assert (length (ans) == 3)

%!test
%! % ensure None is returned if nargout > 0
%! L = pyeval ("[1, None, 3]");
%! a = L{2};
%! assert (char (a), "None")

%!error <outputs must match>
%! % multiple return values: too many outputs
%! f = pyeval ("lambda: (1, 2)");
%! [a, b, c] = f ();

%!error <outputs must match>
%! % multiple return values: not enough outputs
%! f = pyeval ("lambda: (1, 2, 3)");
%! [a, b] = f ();
