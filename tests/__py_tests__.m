## Copyright (C) 2017-2019 Mike Miller
## Copyright (C) 2005-2017 David Bateman
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

## This is a script file, derived from Octave's __run_test_suite__.m
true;

function retval = __run_py_tests__ (varargin)

  files_with_no_tests = {};
  files_with_tests = {};

  pso = page_screen_output ();
  orig_wstate = warning ();

  logfile = make_absolute_filename ("fntests.log");
  unwind_protect
    page_screen_output (false);
    warning ("off", "Octave:data-file-in-path");
    try
      fid = fopen (logfile, "wt");
      if (fid < 0)
        error ("__run_test_suite__: could not open %s for writing", logfile);
      endif
      test ("", "explain", fid);
      dp = dn = dxf = dsk = drtsk = 0;
      puts ("\nIntegrated test scripts:\n\n");
      for i = 1:length (varargin)
        f = varargin{i};
        if (has_tests (f))
          print_test_file_name (f);
          if (nargout ("test") < 5)
            rtsk = 0;
            [p, n, xf, sk] = test (f, "quiet", fid);
          else
            [p, n, xf, sk, rtsk] = test (f, "quiet", fid);
          endif
          print_pass_fail (p, n, xf, sk, rtsk);
          dp += p;
          dn += n;
          dxf += xf;
          dsk += sk;
          drtsk += rtsk;
          files_with_tests(end+1) = f;
        else
          ## To reduce the list length, only mark .cc files that contain
          ## DEFUN definitions.
          files_with_no_tests(end+1) = f;
        endif
      endfor

      puts ("\nSummary:\n\n");
      nfail = dn - dp - dxf;
      printf ("  PASS                         %6d\n", dp);
      printf ("  FAIL                         %6d\n", nfail);
      if (dxf > 0)
        printf ("  XFAIL                        %6d\n", dxf);
      endif
      if (dsk > 0)
        printf ("  SKIPPED (feature)            %6d\n", dsk);
      endif
      if (drtsk > 0)
        printf ("  SKIPPED (run-time condition) %6d\n", drtsk);
      endif
      puts ("\n");
      printf ("See the file %s for additional details.\n", logfile);
      if (dxf > 0)
        puts ("\n");
        puts ("Items listed as XFAIL above are known bugs.\n");
        puts ("Bug report numbers for them may be found in the log file:\n");
        puts (logfile);
        puts ("\nPlease help improve Pythonic by contributing fixes for them.\n");
      endif
      if (dsk > 0 || drtsk > 0)
        puts ("\n");
        puts ("Tests are most often skipped because the features they require\n");
        puts ("have been disabled.  Features are most often disabled because\n");
        puts ("they require dependencies that were not present when Octave or\n");
        puts ("Pythonic was built.\n");
      endif

      report_files_with_no_tests (files_with_tests, files_with_no_tests, ".m");

      puts ("\nPlease help improve Pythonic by contributing tests for these files\n");
      printf ("(see the list in the file %s).\n\n", logfile);

      fprintf (fid, "\nFiles with no tests:\n\n%s",
                    list_in_columns (files_with_no_tests, 80));
      fclose (fid);
    catch
      disp (lasterr ());
    end_try_catch
  unwind_protect_cleanup
    warning ("off", "all");
    warning (orig_wstate);
    page_screen_output (pso);
  end_unwind_protect
  retval = (nfail != 0);
endfunction

function print_test_file_name (nm)
  filler = repmat (".", 1, 60-length (nm));
  printf ("  %s %s", nm, filler);
endfunction

function print_pass_fail (p, n, xf, sk, rtsk)

  if ((n + sk + rtsk) > 0)
    printf (" PASS   %4d/%-4d", p, n);
    nfail = n - p - xf;
    if (nfail > 0)
      printf ("\n%71s %3d", "FAIL ", nfail);
    endif
    if (sk > 0)
      printf ("\n%71s %3d", "(missing feature) SKIP ", sk);
    endif
    if (rtsk > 0)
      printf ("\n%71s %3d", "(run-time condition) SKIP ", rtsk);
    endif
    if (xf > 0)
      printf ("\n%71s %3d", "XFAIL", xf);
    endif
  endif
  puts ("\n");

endfunction

function retval = has_tests (f)

  fid = fopen (f);
  if (fid < 0)
    error ("__run_test_suite__: fopen failed: %s", f);
  endif

  str = fread (fid, "*char")';
  fclose (fid);
  retval = ! isempty (regexp (str,
                              '^%!(assert|error|fail|test|xtest|warning)',
                              'lineanchors', 'once'));

endfunction

function n = num_elts_matching_pattern (lst, pat)
  n = sum (! cellfun ("isempty", regexp (lst, pat, 'once')));
endfunction

function report_files_with_no_tests (with, without, typ)
  pat = ['\' typ "$"];
  n_with = num_elts_matching_pattern (with, pat);
  n_without = num_elts_matching_pattern (without, pat);
  n_tot = n_with + n_without;
  printf ("\n%d (of %d) %s files have no tests.\n", n_without, n_tot, typ);
endfunction

exit (__run_py_tests__ (argv (){:}));

## Mark this file as fully tested.
%!assert (1)
