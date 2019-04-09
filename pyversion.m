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
## @deftypefn  {} {} pyversion
## @deftypefnx {} {@var{ver} =} pyversion
## @deftypefnx {} {[@var{ver}, @var{exec}, @var{loaded}] =} pyversion
## List information about the Python runtime used by Octave Python functions.
## @end deftypefn

function varargout = pyversion ()

  if (nargin > 0)
    print_usage ();
  endif

  ver = char (pycall ("sysconfig.get_python_version"));

  if ((nargout == 0) || (nargout > 1))
    exec = char (py.sys.executable);
  endif

  ## FIXME: determine whether the Python runtime has been loaded yet
  if ((nargout == 0) || (nargout > 2))
    loaded = true;
  endif

  if (nargout == 0)
    dlllibrary = char (pycall ("sysconfig.get_config_var", "DLLLIBRARY"));
    if (isempty (dlllibrary))
      libdir = pycall ("sysconfig.get_config_var", "LIBDIR");
      ldlibrary = pycall ("sysconfig.get_config_var", "LDLIBRARY");
    else
      libdir = pycall ("sysconfig.get_config_var", "BINDIR");
      ldlibrary = dlllibrary;
    endif
    multiarch = pycall ("sysconfig.get_config_var", "MULTIARCH");
    lib = char (pycall ("os.path.join", libdir, multiarch, ldlibrary));
    prefix = char (pycall ("sysconfig.get_config_var", "prefix"));

    printf ("       version: \"%s\"\n", ver);
    printf ("    executable: \"%s\"\n", exec);
    printf ("       library: \"%s\"\n", lib);
    printf ("        prefix: \"%s\"\n", prefix);
    printf ("        loaded: %d\n", loaded);
  else
    if (nargout > 0)
      varargout{1} = ver;
    endif
    if (nargout > 1)
      varargout{2} = exec;
    endif
    if (nargout > 2)
      varargout{3} = loaded;
    endif
  endif

endfunction


%!assert (ischar (pyversion ()))

%!test
%! [m, n] = regexp (pyversion (), '^(\d\.\d)$');
%! assert ([m, n], [1, 3])

%!test
%! [~, ~, v] = pyversion ();
%! assert (v, true)

%!error pyversion (1)
%!error pyversion ("python")
%!error pyversion ("/usr/bin/python")
