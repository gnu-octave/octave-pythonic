## Copyright (C) 2019 Mike Miller
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
## @deftypefn  {} {} pythonic
## @deftypefnx {} {} pythonic help
## @deftypefnx {} {} pythonic gitlab
## @deftypefnx {} {} pythonic issue
## @deftypefnx {} {} pythonic update
## @deftypefnx {} {} pythonic version
## @deftypefnx {} {} pythonic versions
## @deftypefnx {} {} pythonic wiki
## @deftypefnx {} {@var{v} =} pythonic ("version")
## @deftypefnx {} {@var{v} =} pythonic ("versions")
## Display useful information about the Pythonic package.
##
## With no arguments, display a summary description and simple examples
## showing how to use the package. The subcommand @qcode{"help"} is the
## same.
##
## Other available subcommands are
##
## @table @asis
## @item  @qcode{"gitlab"}
## @itemx @qcode{"git"}
## Open the project page on GitLab in the default web browser.
##
## @item @qcode{"wiki"}
## Open the project page on Wiki in the default web browser.
##
## @item  @qcode{"issue"}
## @itemx @qcode{"bug"}
## Open a new issue on GitLab in the default web browser.
##
## @item @qcode{"update"}
## Attempt to update to the latest available release of the Pythonic package.
##
## @item @qcode{"version"}
## Display the currently installed version of the Pythonic package.
##
## @item @qcode{"versions"}
## Display a list of all available releases of the Pythonic package.
## @end table
## @end deftypefn

function varargout = pythonic (command)

  if (nargin > 1)
    print_usage ();
  endif

  if (nargin == 0)
    command = "help";
  endif

  switch (command)
    case "help"
      help ();
    case {"git", "gitl", "gitla", "gitlab"}
      gitlab ();
    case "wiki"
      wiki ();
    case {"iss", "issu", "issue", "bug"}
      issue ();
    case {"up", "upd", "upda", "updat", "update"}
      update ();
    case "version"
      if (nargout == 0)
        version ();
      else
        varargout{1} = version ();
      endif
    case "versions"
      if (nargout == 0)
        versions ();
      else
        varargout{1} = versions ();
      endif
    otherwise
      error ("pythonic: unrecognized subcommand \"%s\", see \"help pythonic\"", command)
  endswitch

endfunction

function help ()
  disp ("")
  disp ("Pythonic is a Python foreign function interface for GNU Octave.")
  disp ("")
  disp ("Use Python types with the 'py' namespace prefix")
  disp ("")
  disp ("    numbers = py.list ({1, 2, 3, 4, 5})")
  disp ("    info = py.dict (uname ())")
  disp ("")
  disp ("Call Python functions in any available module the same way")
  disp ("")
  disp ("    x = py.math.sqrt (2)")
  disp ("    d = py.arrow.arrow.Arrow.now ()")
  disp ("")
  disp ("Use the 'pyversion' function to see which Python is being used.")
  disp ("Use the 'pythonic update' command to install the latest version.")
  disp ("")
endfunction

function gitlab ()
  pythonic_web ("https://gitlab.com/mtmiller/octave-pythonic");
endfunction

function wiki ()
  pythonic_web ("https://wiki.octave.org/Pythonic");
endfunction

function issue ()
  pythonic_web ("https://gitlab.com/mtmiller/octave-pythonic/issues/new");
endfunction

function update ()
  ver_curr = installed_version ();
  [ver_avail, url_avail] = most_recently_released_version ();

  if (compare_versions (ver_curr, ver_avail, "<"))
    disp (sprintf ("Pythonic version %s is available, updating...", ver_avail))
    pkg ("install", url_avail);
  elseif (compare_versions (ver_curr, ver_avail, ">"))
    disp (sprintf ("Pythonic version %s is higher than the latest official release, nothing to do", ver_curr))
  else
    disp (sprintf ("Pythonic version %s is already up to date, nothing to do", ver_curr))
  endif

endfunction

function v = version ()
  ver_curr = installed_version ();
  if (nargout == 0)
    disp (ver_curr)
  else
    v = ver_curr;
  endif
endfunction

function v = versions ()
  data = releases ();
  n = int64 (py.len (data));

  if (nargout != 0)
    v = cell (n, 1);
  endif

  for i = 1:n
    release_name = char (data{n}{"name"});
    if (nargout == 0)
      disp (sprintf ("  %s", release_name))
    else
      v{i} = release_name;
    endif
  endfor

endfunction

function v = installed_version ()
  data = pkg ("list", "pythonic");
  v = char (data{1}.version);
endfunction

function info = releases ()
  gitlab_api = "https://gitlab.com/api/v4";
  project_id = "11728647";

  releases = [gitlab_api "/projects/" project_id "/releases"];

  [json, ok, errmsg] = urlread (releases);

  if (ok)
    info = py.json.loads (json);
  else
    error ("pythonic: unable to retrieve release information: %s", errmsg)
  endif

endfunction

function [v, url] = most_recently_released_version ()
  data = releases ();
  v = char (data{1}{"name"});

  links = data{1}{"assets"}{"links"};
  n = int64 (py.len (links));
  for i = 1:n
    name = char (links{i}{"name"});
    url = char (links{i}{"url"});
    if (strfind (name, "(package)"))
      break
    endif
  endfor

endfunction

function pythonic_web (url)
  try
    web (url);
  catch
    ## FIXME: call internal function as fallback for Octave version < 6
    __open_with_system_app__ (url);
  end_try_catch
endfunction

%!error pythonic ("invalid")
%!error pythonic ("versions", 2)
