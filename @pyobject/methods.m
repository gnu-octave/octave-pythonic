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
## @defmethod @@pyobject methods (@var{x})
## List the properties/callables of a Python object.
##
## Returns a cell array of strings, the names of the ``callables''
## of @var{x}.
##
## Example:
## @example
## @group
## pyexec ("import os")
## os = pyeval ("os");
## methods (os)
##   @print{} Methods for Python module 'os':
##   @print{} ...
##   @print{} chdir ...
##   @print{} ...
## x = methods (os)
##   @result{} x =
##     @{
##       [1,1] = ...
##       [2,1] = ...
##        ...  = chdir
##        ...  = getenv
##        ...
##     @}
## @end group
## @end example
##
## To get the properties (non-callables) of an object,
## @pxref{@@pyobject/fieldnames}.
##
## Note that if you instead want the methods implemented by
## the Octave class @code{@@pyobject}, you can always do:
## @example
## @group
## methods pyobject
##   @print{} Methods for class pyobject:
##   @print{} display  ...  subsref
##   @print{} ...
## @comment this doctest may need updating as we add methods
## @end group
## @end example
##
## @seealso{methods, @@pyobject/fieldnames}
## @end defmethod


function mtds = methods (x)

  # filter the output of `dir(x)` to get callable methods only
  cmd = pyeval (["lambda x: [a for a in dir(x)" ...
                 " if callable(getattr(x, a)) and not a.startswith('_')]"]);

  # FIXME: may need to convert from Python list to Octave cell array
  mtds_list = pycall (cmd, x)

  if (nargout == 0)
    ## FIXME: should this be available as @pyobject/ismodule.m ?
    is_module = pyeval ("lambda x: isinstance(x, __import__('types').ModuleType)");

    if (pycall (is_module, x))
      modulename = pycall ("getattr", x, "__name__");
      printf ("Methods for Python module '%s':\n", modulename);
    else
      ## FIXME: should be `class (x)`
      classref = pycall ("getattr", x, "__class__");
      classname = pycall ("getattr", classref, "__name__");
      printf ("Methods for Python class '%s':\n", classname);
    endif
    disp (list_in_columns (mtds_list));
  else
    mtds = mtds_list(:);
  endif

endfunction


%!test
%! sys = pycall ("__import__", "sys");
%! m = methods (sys);
%! assert (iscellstr (m))
%! assert (any (strcmp (m, "exit")))

%!test
%! os = pycall ("__import__", "os");
%! m = methods (os);
%! assert (iscellstr (m))
%! assert (any (strcmp (m, "chdir")))
%! assert (any (strcmp (m, "getcwd")))
%! assert (any (strcmp (m, "getenv")))
%! assert (any (strcmp (m, "getpid")))
