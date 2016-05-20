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
%%
%% @seealso{pyexec, pyeval}
%% @end defun

classdef pyobj < handle
  properties
    id
  end

  methods(Static)
    function x = fromPythonVarName(pyvarname)
      % if @var{pyvarname} is a string, its assumed to be a variable
      % name, e.g., previously created with pyexec.  This must exist
      % at the time of construction but it can disappear later (we
      % will keep track of the reference).
      if (~ ischar(pyvarname))
        error('pyobj: currently we only take variable names as input')
      end
      cmd = sprintf ([ ...
        'if not ("__InOct__" in vars() or "__InOct__" in globals()):\n' ...
        '  __InOct__ = dict()\n' ...
        '__InOct__[hex(id(%s))] = %s' ], ...
        pyvarname, pyvarname);
      pyexec (cmd);
      id = pyeval (['hex(id(' pyvarname '))']);
      x = pyobj(id);
    end
  end


  methods
    function x = pyobj(id)
      % warning: not intended for casual use: you must also insert
      % the object into the Python `__InOct__` dict with key `id`.
      x.id = id;
    end

    function delete(x)
      % called on clear of the last reference---for subclasses of
      % handle; not called at all for "value classes".
      % FIXME: #46497 this is never called!
      %save('proof_of_delete', 6, x.id)
      disp ('delete')
      % throws KeyError if it wasn't in there for some reason
      cmd = sprintf ('__InOct__.pop("%s")', x.id);
      pyexec (cmd);
    end

    function force_delete (x)
      % Manual workaround for #46497: call right before @code{clear x}.  But
      % be careful, @code{x} needs to be the last reference: don't do this:
      % @example
      % d = pyobj (...);
      % d2 = d;
      % force_delete (d)
      % clear d
      % d2
      %   @print{} ... KeyError ...
      % @end example
      delete(x)
    end

    dummy (x)

    function r = getid (x)
      r = x.id;
    end

    function disp(x)
      printf ('[PyObject id %s]\n', x.id);
      disp (pyeval (sprintf ('str(__InOct__["%s"])', x.id)))
    end

    function s = whatclass(x)
      s = pyeval (sprintf ('str(__InOct__["%s"].__class__)', x.id));
    end

    function lst = whatmethods(x)
      % filter the output of `dir(x)`
      % properties only:
      % [a for a in dir(x) if not callable(getattr(x, a)) and not a.startswith("__")]
      cmd = sprintf ( ...
        '[a for a in dir(__InOct__["%s"]) if not a.startswith("__")]', ...
        x.id);
      lst = pyeval (cmd);
    end

    function r = subsref(x, idx)
      switch idx.type
        case '()'
          if ( ~strcmp (idx.subs, ''))
	    idx
            error('not implemented: function calls with arguments')
          end
          r = pyeval (sprintf ('__InOct__["%s"]()', x.id));
        case '.'
          assert(ischar(idx.subs))
          r = pyeval (sprintf ('__InOct__["%s"].%s', x.id, idx.subs));
        otherwise
          idx
          error('not implemented')
      end
    end

  end
end
