## Copyright (C) 2016 Colin B. Macdonald
## Copyright (C) 2017 NVS Abhilash
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
## @defmethod  @@pyobject methods (@var{x})
## @defmethodx @@pyobject methods (@var{x}, "-all")
## List the properties/callables of a Python object.
##
## Returns a cell array of strings, the names of the ``callables''
## of @var{x}.
##
## If provided with an option @qcode{"-all"}, private methods are also included.
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
##   @print{} ...   display   ...
## @comment this doctest may need updating as we add methods
## @end group
## @end example
##
## @seealso{methods, @@pyobject/fieldnames}
## @end defmethod


function mtds = methods (x, option)

  if (nargin < 1 || nargin > 2)
    print_usage ();
  endif

  show_all = false;
  if (nargin == 2)
    if (ischar (option))
      switch (tolower (option))
        case "-all"
          show_all = true;
        otherwise
          warning ("methods: unrecognized OPTION '%s'", option);
      endswitch
    else
      error ("methods: OPTION must be a string");
    endif
  endif

  query_end = "";
  if (! show_all)
    query_end = "and not a.startswith('_')";
  endif

  query = sprintf (["lambda x: [a for a in dir(x)" ...
                    " if callable(getattr(x, a)) %s]"], query_end);

  cmd = pyeval (query);

  mtds_list_obj = pycall (cmd, x);

  mtds_list = cellfun (@char, cell (mtds_list_obj), "uniformoutput", false);

  if (nargout == 0)
    if (isa (x, "py.types.ModuleType"))
      modulename = char (pycall ("getattr", x, "__name__"));
      printf ("Methods for Python module '%s':\n", modulename);
    else
      printf ("Methods for Python class '%s':\n", __py_class_name__ (x));
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

%!assert (methods (pyeval ("object()")), cell (0, 1))
%!assert (ismember ("append", methods (pyeval ("[]"))))
%!assert (ismember ("keys", methods (pyeval ("{}"))))

%!assert (! ismember ("__reversed__", methods (pyeval ("[]"))))
%!assert (ismember ("__reversed__", methods (pyeval ("[]"), "-all")))
%!assert (! ismember ("__contains__", methods (pyeval ("{}"))))
%!assert (ismember ("__contains__", methods (pyeval ("{}"), "-all")))
