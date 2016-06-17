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
%% @defun  pyobject (@var{s})
%% Wrap a Python object.
%%
%% TODO: where/how to document classdef classes?
%%
%% @seealso{pyexec, pyeval}
%% @end defun

classdef pyobject < handle
  properties
    id
  end

  methods (Static)
    function x = fromPythonVarName (pyvarname)
      % if @var{pyvarname} is a string, its assumed to be a variable
      % name, e.g., previously created with pyexec.  This must exist
      % at the time of construction but it can disappear later (we
      % will keep track of the reference).
      if (! ischar(pyvarname))
        error('pyobject: currently we only take variable names as input')
      end
      cmd = sprintf ([ ...
        'if not ("__InOct__" in vars() or "__InOct__" in globals()):\n' ...
        '    __InOct__ = dict()\n' ...
        '    # FIXME: make it accessible elsewhere?\n' ...
        '    import __main__\n' ...
        '    __main__.__InOct__ = __InOct__\n' ...
        '__InOct__[hex(id(%s))] = %s' ], ...
        pyvarname, pyvarname);
      pyexec (cmd);
      id = pyeval (['hex(id(' pyvarname '))']);
      x = pyobject (id);
    end
  end


  methods
    function x = pyobject (id)
      % warning: not intended for casual use: you must also insert
      % the object into the Python `__InOct__` dict with key `id`.
      x.id = id;
    end

    function delete (x)
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
      % d = pyobject (...);
      % d2 = d;
      % force_delete (d)
      % clear d
      % d2
      %   @print{} ... KeyError ...
      % @end example
      delete (x)
    end

    dummy (x)
    display (x)
    subsref (x, idx)

    function r = getid (x)
      r = x.id;
    end

    function varargout = disp (x)
      s = pyeval (sprintf ('str(__InOct__["%s"])', x.id));
      if (nargout == 0)
        disp (s)
      else
        varargout = {s};
      end
    end

    function s = whatclass (x)
      s = pyeval (sprintf ('str(__InOct__["%s"].__class__)', x.id));
    end

    function vargout = help (x)
      idx = struct ('type', '.', 'subs', '__doc__');
      s = subsref (x, idx);
      if (nargout == 0)
        disp (s)
      else
        vargout = {s};
      end
    end

  end
end
