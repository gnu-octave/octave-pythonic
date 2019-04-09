## Copyright (C) 2016 Colin B. Macdonald
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
  switch (t.type)
    case "."
      assert (ischar (t.subs))
      r = pycall ("getattr", x, t.subs);

    case "()"
      ## Determine the types and protocols that we are able to index into
      x_is_callable = __py_isinstance__ (x, "py.collections.Callable");
      x_is_sequence = __py_isinstance__ (x, "py.collections.Sequence") ...
                      | __py_isinstance__ (x, "py.array.array") ...
                      | __py_isinstance__ (x, "py.numpy.ndarray");

      if (! (x_is_callable || x_is_sequence))
        error ("subsref: cannot index Python object, not sequence or callable");
      endif

      if (x_is_sequence)
        error ("subsref: slice indexing of Python objects not yet implemented");
      endif

      r = pycall (x, t.subs{:});

    case "{}"
      ## Determine the types and protocols that we are able to index into
      x_is_mapping = __py_isinstance__ (x, "py.collections.Mapping");
      x_is_sequence = __py_isinstance__ (x, "py.collections.Sequence") ...
                      | __py_isinstance__ (x, "py.array.array") ...
                      | __py_isinstance__ (x, "py.numpy.ndarray");

      if (! (x_is_mapping || x_is_sequence))
        error ("subsref: cannot index Python object, not sequence or mapping");
      endif

      ## Subtract one from index: do this for lists, arrays, numpy arrays, etc
      for i = 1:length (t.subs)
        j = t.subs{i};
        if (isindex (j) && isnumeric (j) && x_is_sequence)
          t.subs{i} = cast (j, class (sizemax ())) - 1;
        endif
      endfor

      if (ischar (t.subs{1}) && strcmp (t.subs{1}, ":"))
        if (x_is_mapping)
          ind = ":";
        else
          ind = int32 ([1:length(x)] - 1);
        endif
      elseif (isscalar (t.subs))
        ind = t.subs{1};
      else
        ind = pycall ("tuple", t.subs);
      endif

      if (isempty (ind) && x_is_sequence)
        r = pyobject ();
      elseif (isnumeric (ind) && length (ind) > 1)
        r = {};
        for k = 1:length (ind)
          r(end+1) = pycall ("operator.getitem", x, ind(k));
        endfor
      else
        r = pycall ("operator.getitem", x, ind);
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
  if (nargout == 0 && ! __py_is_none__ (r))
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
%! L = pyeval ("[10., 20.]");
%! assert (L{1}, 10)
%! assert (L{2}, 20)

%!test
%! % list indexing, slice
%! L = pyeval ("[10., 20., [30., 40.]]");
%! L2 = L{:};
%! assert (L2{1}, 10)
%! assert (L2{2}, 20)
%! assert (L2{3}{1}, 30)
%! assert (L2{3}{2}, 40)

%!test
%! % list indexing, nested list
%! L = pyeval ("[1., 2., [10., 11., 12.]]");
%! assert (L{2}, 2)
%! assert (L{3}{1}, 10)
%! assert (L{3}{3}, 12)

%!test
%! % list indexing, assign to vars
%! L = pyeval ("[1., 2., 'Octave']");
%! [a, b, c] = L{:};
%! assert (a, 1)
%! assert (b, 2)
%! assert (char (c), "Octave")

## Test that depends on implicit creation of NumPy arrays, do we want this?
%!xtest
%! % 2D array indexing
%! A = pyobject ([1. 2.; 3. 4.]);
%! assert (A{1, 1}, 1)
%! assert (A{2, 1}, 3)
%! assert (A{1, 2}, 2)

## Test element indexing on array.array types
%!test
%! a = pycall ("array.array", "d", {11, 12, 13, 14});
%! assert (a{1}, 11)
%! assert (a{2}, 12)
%! assert (a{end}, 14)

%!test
%! % dict: str key access
%! d = pyeval ("{'one':1., 5:5, 6:6}");
%! assert (d{"one"}, 1)

%!test
%! % dict: integer key access
%! d = pyeval ("{5:42., 6:42.}");
%! assert (d{6}, 42)

%!test
%! % dict: integer key should not subtract one
%! d = pyeval ("{5:40., 6:42.}");
%! assert (d{6}, 42)

%!test
%! % dict: floating point keys should work
%! d = pyeval ("{5.5:'ok'}");
%! assert (char (d{5.5}), "ok")

%!test
%! % dict: make sure key ":" doesn't break anything
%! d = pyeval ("{'a':1., ':':2.}");
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
%! s = pyeval ("set({1., 2.})");
%! v = s.pop ();
%! assert (length (s) == 1)
%! assert (v == 1 || v == 2)

%!test
%! % chain
%! pyexec ("import sys")
%! s = pyeval ("set({sys})");
%! ver = s.pop ().version;
%! assert (isa (ver, "pyobject"))
%! assert (ischar (char (ver)))

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
%! f = pyeval ("lambda: (1., 2., 3.)");
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

## Test of multi-element indexing, fails to return correct number of output args
%!xtest
%! a = {1, 2, 3, 4, 5, 6};
%! b = pyobject (a);
%! b{:};
%! assert (ans, a{end})

%!xtest
%! a = {1, 2, 3, 4, 5, 6};
%! b = pyobject (a);
%! c = {b{:}};
%! assert (c, a)

%!error <cannot index Python object>
%! f = pyeval ("abs");
%! f{1}

%!error <outputs must match>
%! % multiple return values: too many outputs
%! f = pyeval ("lambda: (1, 2)");
%! [a, b, c] = f ();

%!error <outputs must match>
%! % multiple return values: not enough outputs
%! f = pyeval ("lambda: (1, 2, 3)");
%! [a, b] = f ();
