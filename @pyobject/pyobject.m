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
## @defun  pyobject (@var{s})
## Wrap a Python object.
##
## TODO: where/how to document classdef classes?
##
## @seealso{pyexec, pyeval}
## @end defun


classdef pyobject < handle
  properties
    m_id
  endproperties


  methods
    function obj = pyobject (x, id)
      if (nargin == 0)
        obj = pyeval ("None");
      elseif (nargin == 1)
        ## Convert the input to a pyobject
        if (isa (x, "pyobject"))
          obj = x;
        else
          obj.m_id = __py_objstore_put__ (x);
        endif
      elseif (nargin == 2)
        ## The actual constructor.  Nicer to split this off to static method
        ## like `pyobject.new` but I don't know how to call from pycall.cc.
        ## Warning: not intended for casual use: you must also insert the
        ## object into the Python object store with key `id`.
        obj.m_id = id;
      else
        error ("pyobject: unexpected input to the constructor")
      endif
    endfunction

    function delete (x)
      # Called on clear of the last reference---for subclasses of
      # handle; not called at all for "value classes".
      #
      # FIXME: #46497 this is never called!
      # Workaround: call @code{delete(x)} right before @code{clear x}.  But
      # be careful, @code{x} needs to be the last reference: don't do this:
      # @example
      # d = pyobject (...);
      # d2 = d;
      # delete (d)
      # clear d
      # d2
      #   @print{} ... KeyError ...
      # @end example

      #disp ("delete")

      __py_objstore_del__ (x.m_id);
    endfunction

    # methods defined in external files
    dummy (x)
    display (x)
    subsref (x, idx)

    function r = id (x);
      r = x.m_id;
    endfunction

    function s = char (x)
      s = pycall ("str", x);
    endfunction

    function varargout = disp (x)
      s = char (x);
      if (nargout == 0)
        disp (s)
      else
        varargout = {s};
      endif
    endfunction

    function s = class (x)
      idx = struct ("type", ".", "subs", "__class__");
      class_ref = subsref (x, idx);
      idx = struct ("type", ".", "subs", "__name__");
      s = subsref (class_ref, idx);
      s = sprintf ("py.%s", char (s));
    endfunction

    function y = double (x)
      fn = pyeval ("lambda x: isinstance(x, __import__('array').array)");
      if (pycall (fn, x))
        c = cell (x);
        y = cellfun (@(t) eval ("double (t)"), c);
      else
        y = pycall ("float", x);
      endif
    endfunction

    function y = int64 (x)
      y = __py_int64_scalar_value__ (x);
    endfunction

    function y = isa (x, typestr)
      assert (nargin == 2);
      assert (ischar (typestr));
      if ((numel (typestr) > 3) && (typestr(1:3) == "py."))
        y = __py_isinstance__ (x, typestr);
      else
        y = builtin ("isa", x, typestr);
      endif
    endfunction

    function y = struct (x)
      y = __py_struct_from_dict__ (x);
    endfunction

    function vargout = help (x)
      idx = struct ("type", ".", "subs", "__doc__");
      s = subsref (x, idx);
      if (nargout == 0)
        disp (s)
      else
        vargout = {s};
      endif
    endfunction

    function len = length (x)
      try
        len = double (pycall ("len", x));
      catch
        len = 1;
      end_try_catch
    endfunction


    function [n, varargout] = size (x, d)
      assert (nargin <= 2)
      try
        idx = struct ("type", ".", "subs", "shape");
        sz = subsref (x, idx);
        sz = cellfun (@(x) eval ("double (x)"), cell (sz));
      catch
        ## if it had no shape, make it a row vector
        sz = [1 length(x)];
      end_try_catch

      ## simplest case
      if (nargout <= 1 && nargin == 1)
        n = sz;
        return
      endif

      ## quirk: pad extra dimensions with ones
      if (nargin < 2)
        d = 1;
      endif
      sz(end+1:max (d,nargout-end)) = 1;

      if (nargin > 1)
        assert (nargout <= 1)
        n = sz(d);
        return
      endif

      ## multiple outputs
      n = sz(1);
      for i = 2:(nargout-1)
        varargout{i-1} = sz(i);
      endfor
      ## last is product of all remaining
      varargout{nargout-1} = prod (sz(nargout:end));
    endfunction


    function n = numel (x)
      assert (nargin == 1)
      sz = size (x);
      n = prod (sz);
    endfunction

    function n = ndims (x)
      assert (nargin == 1)
      n = length (size (x));
    endfunction

    function r = end (x, index_pos, num_indices)
      assert (nargin == 3)
      assert (isscalar (index_pos))
      if (num_indices == 1)
        r = numel (x);
      else
        r = size (x, index_pos);
      endif
    endfunction

    function res = isequal (varargin)
      assert (nargin >= 2)
      res = all (strcmp ("pyobject", cellfun ("class", varargin, "uniformoutput", false)));
      for i = 2:nargin
        if (! res)
          return;
        endif
        res = res && pycall("bool", pycall ("operator.eq", varargin{1}, varargin{i}));
      endfor
    endfunction
  endmethods
endclassdef


%!test
%! pyexec ("import sys")
%! A = pyeval ("sys");
%! assert (isa (A, "pyobject"))

%!test
%! pyobj = pyeval ("{1:2, 2:3, 3:4}");
%! assert (isa (pyobj, "pyobject"))
%! assert (length (pyobj), 3)

%!test
%! pyexec ("import sys");
%! pyobj = pyeval ("sys");
%! assert (length (pyobj), 1)

%!assert (size (pyeval ("[10, 20, 30]")), [1 3])
%!assert (size (pyeval ("[10, 20, 30]"), 1), 1)
%!assert (size (pyeval ("[10, 20, 30]"), 2), 3)
%!assert (size (pyeval ("[10, 20, 30]"), 3), 1)

%!test
%! L = pyeval ("[10, 20, 30]");
%! a = size (L);
%! assert (a, [1, 3])
%! [a b] = size (L);
%! assert ([a b], [1 3])
%! [a b c] = size (L);
%! assert ([a b c], [1 3 1])

%!assert (numel (pyeval ("[10, 20, 30]")), 3)

%!test
%! L = pyeval ("[10, 20, 30]");
%! assert (double (L{end}), 30)
%! assert (double (L{end-1}), 20)

%!test
%! % ensure "end" works for iterables that are not lists
%! myrange = pyeval ( ...
%!   "range if __import__('sys').hexversion >= 0x03000000 else xrange");
%! R = pycall (myrange, int32 (5), int32 (10), int32 (2));
%! assert (double (R{end}), 9)

%!shared a
%! pyexec ("class _myclass(): shape = (3, 4, 5)")
%! a = pyeval ("_myclass()");
%!assert (size (a), [3 4 5])
%!assert (size (a, 3), 5)
%!test
%! s = size (a);
%! assert (s, [3 4 5])
%!test
%! [n m] = size (a);
%! assert ([n m], [3 20])
%!test
%! [n m o] = size (a);
%! assert ([n m o], [3 4 5])
%!test
%! [n m o p] = size (a);
%! assert ([n m o p], [3 4 5 1])
%!assert (numel (a), 60)
%!assert (ndims (a), 3)
%!shared

## Test conversion of scalar struct into a Python dict
%!shared s1
%! s1.name = "Octave";
%! s1.value = 42;
%! s1.counts = {1, 2, 3};
%! s1.ok = true;
%!assert (isa (pyobject (s1), "pyobject"))
%!assert (class (pyobject (s1)), "py.dict")
%!assert (char (pyobject (s1){"name"}), "Octave")
%!assert (pyobject (s1){"value"}, 42)
%!assert (pyobject (s1){"ok"}, true)

%!error pyobject (struct ("a", {}))
%!error pyobject (struct ("a", {1, 2}))

%!assert (char (pyeval ("None")), "None")
%!assert (char (pyeval ("'this is a string'")), "this is a string")
%!assert (char (pyeval ("[1, 2, 3, 4, 5]")), "[1, 2, 3, 4, 5]")
%!assert (char (pyeval ("(1, 2, 3, 4, 5)")), "(1, 2, 3, 4, 5)")
%!assert (char (pyeval ("__import__('sys')")), "<module 'sys' (built-in)>")

%!assert (isa (pyobject (), "pyobject"))
%!assert (isa (pyobject ("a string"), "pyobject"))
%!assert (isa (pyobject (42.2), "pyobject"))
%!assert (isa (pyobject (int32 (42)), "pyobject"))
%!assert (isa (pyobject (pyobject ()), "pyobject"))

%!assert (class (pyeval ("{}")), "py.dict")
%!assert (class (pyeval ("[]")), "py.list")
%!assert (class (pyeval ("()")), "py.tuple")
%!assert (class (pyeval ("set()")), "py.set")
%!assert (class (pyeval ("None")), "py.NoneType")
%!assert (class (pyeval ("2.5")), "double")

## Test conversion method pyobject.double
%!assert (double (pyobject (2.5)), 2.5)
%!assert (double (pyobject (42)), 42)
%!assert (double (pyobject ("42")), 42)
%!assert (double (pyobject (false)), 0)
%!assert (double (pycall ("array.array", "d", {31, 32, 33, 34})), [31, 32, 33, 34])

%!error double (pyobject ("this is not a number"))
%!error double (pyobject ())
%!error double (pyeval ("[1, 2, 3]"))

## Test class type check method pyobject.isa
%!assert (isa (pyobject (), "handle"))
%!assert (isa (pyobject (), "pyobject"))
%!assert (! isa (pyobject (), "py.None"))
%!assert (isa (pyobject (0), "handle"))
%!assert (isa (pyobject (0), "pyobject"))
%!assert (isa (pyobject (0), "py.float"))
%!assert (isa (pyobject (int32 (0)), "py.int"))
%!assert (isa (pyobject (true), "py.bool"))
%!assert (isa (pyobject ("a string"), "py.str"))
%!assert (isa (pyobject (struct ()), "py.dict"))
%!assert (isa (pyobject (cell ()), "py.tuple"))
%!assert (isa (pyobject ([1, 2, 3, 4]), "py.array.array"))

## Test conversion method pyobject.int64
%!assert (int64 (pyobject (int8 (0))), int64 (0))
%!assert (int64 (pyobject (int64 (42))), int64 (42))
%!assert (int64 (pyobject (intmax ("int64"))), intmax ("int64"))
%!assert (int64 (pyobject (intmin ("int64"))), intmin ("int64"))
%!assert (int64 (pycall ("int", 1e100)), intmax ("int64"))
%!assert (int64 (pycall ("int", -1e100)), intmin ("int64"))

## Test conversion method pyobject.struct
%!assert (struct (pycall ("dict")), struct ())
%!assert (struct (pyobject (struct ())), struct ())
%!test
%! a = struct ("a", 1, "b", 2, "three", 3);
%! b = pyobject (a);
%! c = struct (b);
%! assert (c, a)
%!test
%! a = struct ("a", 1, "b", 2, "three", 3);
%! b = pycall ("dict", pyargs ("a", 1, "b", 2, "three", 3));
%! c = struct (b);
%! assert (c, a)

## Octave fails to resolve function overloads via function handles
%!xtest
%! fn = @double;
%! x = pyobject (int64 (42));
%! assert (fn (x), double (x))

%!error (isequal (pyobject ()))
%!assert (! isequal (pyobject (1.2), 1.2))
%!assert (isequal (pyobject ("a string"), pyobject ("a string")))
%!assert (isequal (pyeval ("None"), pyeval ("None")))
%!assert (! isequal (pyeval ("None"), pyeval ("None"), pyobject (10)))
%!assert (isequal (pyobject (10), pyobject (10.0), pyobject (int8 (10))))

%!test
%! A = pyeval ("[1, 2, 3]");
%! B = pycall ("list", {1, 2, 3});
%! assert (isequal (A, B))
