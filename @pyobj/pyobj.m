%% Copyright (C) 2016 Colin B. Macdonald
%%
%% This file is part of PyTave.
%%
%% OctSymPy is free software; you can redistribute it and/or modify
%% it under the terms of the GNU General Public License as published
%% by the Free Software Foundation; either version 3 of the License,
%% or (at your option) any later version.
%%
%% This software is distributed in the hope that it will be useful,
%% but WITHOUT ANY WARRANTY; without even the implied warranty
%% of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
%% the GNU General Public License for more details.
%%
%% You should have received a copy of the GNU General Public
%% License along with this software; see the file COPYING.
%% If not, see <http://www.gnu.org/licenses/>.

%% -*- texinfo -*-
%% @documentencoding UTF-8
%% @defun  pyobj (@var{s})
%% Wrap a Python object.
%%
%% TODO: where/how to document classdef classes?
%%
%% Example:
%% @example
%% @group
%% pyexec('g = 6')
%% g = pyobj('g');
%%
%% sort(whatmethods(g))
%%   @result{} ans =
%%     @{
%%       [1,1] = denominator
%%       [1,2] = imag
%%       [1,3] = numerator
%%       [1,4] = real
%%      @}
%%
%% g.numerator
%%   @result{} ans =  6
%% g.denominator
%%   @result{} ans =  1
%% @end group
%% @end example
%%
%% TODO: this copies the name of the variable back to Octave.  Not
%% robust.  What is a good mechanism?
%%
%%
%% @seealso{pyexec, pyeval}
%% @end defun

classdef pyobj
  properties
    varname
    repr
  end
  methods

    function x = pyobj(pyvarname)
      % @var{pyvarname} should be created with pyexec
      x.varname = pyvarname;
      x.repr = pyeval(['repr(' x.varname ')']);
    end

    function disp(x)
      str = sprintf('PyObject with repr:\n  %s\noriginal variable: %s', x.repr, x.varname);
      disp(str)
    end

    function s = whatclass(x)
      s = pyeval(['str(' x.varname '.__class__)']);
    end

    function lst = whatmethods(x)
      % filter the output of `dir(x)`
      cmd = sprintf(['[a for a in dir(%s) if not callable(getattr(%s, a))' ...
                     ' and not a.startswith("__")]'], ...
                    x.varname, x.varname);
      lst = pyeval(cmd);
    end

    function r = subsref(x, idx)
      switch idx.type
        case '()'
          error('not implemented: () indexing')
        case '.'
          assert(ischar(idx.subs))
          r = pyeval(sprintf('%s.%s', x.varname, idx.subs));
        otherwise
          idx
          error('not implemented')
      end
    end

  end
end
