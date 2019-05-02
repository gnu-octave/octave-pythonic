# Maintainer makefile for Octave Pythonic
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright (C) 2019 Mike Miller
#
# This file is part of Octave Pythonic.
#
# Octave Pythonic is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Octave Pythonic is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Octave Pythonic; see the file COPYING.  If not, see
# <https://www.gnu.org/licenses/>.

OCTAVE ?= octave

all:
	$(MAKE) -C src $@

check: all
	$(OCTAVE) --no-history --no-window-system --norc --silent \
	  --path="$(CURDIR)/inst" --path="$(CURDIR)/src" --path="$(CURDIR)/tests" \
	  tests/__py_tests__.m \
	  $(shell cd inst && LC_ALL=C.UTF-8 ls *.m @*/*.m) \
	  $(shell cd tests && LC_ALL=C.UTF-8 ls *.m) \
	  $(shell cd src && LC_ALL=C.UTF-8 ls *-tst)

clean distclean maintainer-clean mostlyclean:
	$(MAKE) -C src $@

test: check

.PHONY: all check clean distclean maintainer-clean mostlyclean test
