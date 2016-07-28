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
    id
  endproperties

  methods (Static)
    function x = fromPythonVarName (pyvarname)
      # Warning: just for testing, may be removed without notice!
      # If @var{pyvarname} is a string, its assumed to be a variable
      # name, e.g., previously created with pyexec.  This must exist
      # at the time of construction but it can disappear later (we
      # will keep track of the reference).
      if (! ischar (pyvarname))
        error ("pyobject: currently we only take variable names as input")
      endif
      cmd = sprintf ([ ...
        'if not ("__InOct__" in vars() or "__InOct__" in globals()):\n' ...
        '    __InOct__ = dict()\n' ...
        '    # FIXME: make it accessible elsewhere?\n' ...
        '    import __main__\n' ...
        '    __main__.__InOct__ = __InOct__\n' ...
        '__InOct__[hex(id(%s))] = %s' ], ...
        pyvarname, pyvarname);
      pyexec (cmd);
      id = pyeval (["hex(id(" pyvarname "))"]);
      x = pyobject (0, id);
    endfunction
  endmethods


  methods
    function obj = pyobject (x, id)
      if (nargin == 0)
        obj = pyeval ("None");
        return
      endif

      if (nargin == 1)
        ## Convert the input to a pyobject
        if (isa (x, "pyobject"))
          obj = x;
        else
          ## XXX: perhaps not the ideal implementation!
          vars = pyeval ("__import__('__main__').__dict__");
          ## this is vars{"_temp"} = x
          idx = struct ("type", "{}", "subs", {{"_temp"}});
          vars = subsasgn (vars, idx, x);
          cmd = [ ...
            "if not ('__InOct__' in vars() or '__InOct__' in globals()):\n" ...
            "    __InOct__ = dict()\n" ...
            "    # FIXME: make it accessible elsewhere?\n" ...
            "    import __main__\n" ...
            "    __main__.__InOct__ = __InOct__\n" ...
            "__InOct__[hex(id(_temp))] = _temp" ];
          pyexec (cmd);
          id = pyeval (["hex(id(_temp))"]);
          obj = pyobject (0, id);
        endif
        return
      endif

      if (nargin == 2)
        ## The actual constructor.  Nicer to split this off to static method
        ## like `pyobject.new` but I don't know how to call from pycall.cc.
        ## Warning: not intended for casual use: you must also insert the
        ## object into the Python `__InOct__` dict with key `id`.
        obj.id = id;
        return
      endif

      error ("pyobject: unexpected input to the constructor")
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

      # throws KeyError if it wasn't in there for some reason
      cmd = sprintf ("__InOct__.pop('%s')", x.id);
      pyexec (cmd)
    endfunction

    # methods defined in external files
    dummy (x)
    display (x)
    subsref (x, idx)

    function r = getid (x)
      r = x.id;
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

    function s = whatclass (x)
      s = pyeval (sprintf ("str(__InOct__['%s'].__class__)", x.id));
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
        len = pycall ("len", x);
      catch
        len = 1;
      end_try_catch
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
