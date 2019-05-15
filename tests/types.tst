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

%!test <*https://gitlab.com/mtmiller/octave-pythonic/issues/14>
%! x = py.list ({0, 1, 2, 3});
%! assert (py.math.sqrt (2), sqrt (2))
%! assert (! isa (x, "py.no.such.object"))
%! assert (py.math.sqrt (2), sqrt (2))
