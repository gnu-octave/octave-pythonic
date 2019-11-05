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

%!shared id, s
%! s = "This is a unique string for testing";
%! id = __py_objstore_put__ (s);

%!test
%! list = __py_objstore_list__ ();
%! assert (isstruct (list))
%! fields = fieldnames (list);
%! assert (any (strcmp (fields, "count")))
%! assert (any (strcmp (fields, "key")))
%! assert (any (strcmp (fields, "type")))
%! assert (any (strcmp (fields, "value")))

%!test
%! list = __py_objstore_list__ ();
%! idx = find ([list.key] == id);
%! assert (numel (idx), 1)
%! assert (list(idx).count, uint64 (1))
%! assert (list(idx).key, id)
%! assert (list(idx).type, "str")
%! assert (list(idx).value, s)

%!test
%! __py_objstore_drop__ (id);
%! list = __py_objstore_list__ ();
%! idx = find ([list.key] == id);
%! assert (isempty (idx))
