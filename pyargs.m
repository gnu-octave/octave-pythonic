## Copyright (C) 2016 Mike Miller
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
## @deftypefn  {} {} pyargs (@var{name}, @var{value})
## @deftypefnx {} {} pyargs (@var{name}, @var{value}, @dots{})
## Construct a set of Python keyword arguments suitable for passing to
## a Python function.
##
## Python keyword arguments are similar to optional named parameters accepted
## by some Octave functions such as @code{cellfun} or @code{set}.
##
## For example, a dictionary can be initialized with
##
## @example
## @group
## py.dict (pyargs ("one", 1, "two", 2))
##       @result{} [Python object of type dict]
##           @{...@}
## sort (cellfun (@@char, cell (py.list (ans.keys ())), "uniformoutput", false))
##       @result{}
##           @{
##             [1,1] = one
##             [1,2] = two
##           @}
## @end group
## @end example
##
## And a list can be sorted in reverse order with the @code{reverse} keyword
##
## @example
## @group
## x = py.list (num2cell (int32 ([1, 2, 3])));
## x.sort (pyargs ("reverse", true));
## x
##       @result{} x = [Python object of type list]
##           [3, 2, 1]
## @end group
## @end example
## @end deftypefn

function args = pyargs (varargin)

  if (mod (nargin, 2) != 0)
    error ("pyargs: must be called with NAME, VALUE pairs of arguments");
  endif

  keys_vals = {};
  for i = 1:(nargin / 2)
    k = varargin{2*i - 1};
    v = varargin{2*i};
    if (! (ischar (k) && isrow (k)))
      error ("pyargs: NAME %d must be a string", i);
    endif
    try
      v = pyobject (v);
    catch
      error ("pyargs: VALUE %d must be convertible to a Python object", i);
    end_try_catch

    keys_vals{i} = {k, v};
  endfor

  ## This function needs to return a Python mapping object that will be
  ## recognized by other functions as containing keyword arguments.
  ## Define the class using a type specifier so it can be defined and
  ## instantiated as a single expression.
  class_name = "_OctaveKwargs";
  props = "{'is_kwargs_argument': False}";
  args = pyeval (sprintf ("type('%s', (dict,), %s)()", class_name, props));
  args.update (keys_vals);
  args.is_kwargs_argument = true;

endfunction


%!assert (isa (pyargs (), "pyobject"))
%!assert (cell (py.list (py.dict (pyargs ()).keys ())), cell (1, 0))
%!assert (sort (cellfun (@char, cell (py.list (py.dict (pyargs ("one", 1)).keys ())), "uniformoutput", false)), {"one"})
%!assert (sort (cellfun (@char, cell (py.list (py.dict (pyargs ("one", 1, "two", 2)).keys ())), "uniformoutput", false)), {"one", "two"})

%!error pyargs (1)
%!error pyargs (1, 2)
%!error pyargs ("one")
