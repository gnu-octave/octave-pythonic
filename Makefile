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

PACKAGE := $(shell sed -n 's/^Name: \([-+.~0-9A-Za-z]\+\)/\1/p' DESCRIPTION)
VERSION := $(shell sed -n 's/^Version: \([-+.~0-9A-Za-z]\+\)/\1/p' DESCRIPTION)
TARNAME = octave-$(PACKAGE)-$(VERSION)

# Support out-of-tree builds with 'make O=objdir' and read-only source tree
ifdef O
OBJDIR = $(realpath $(O))
SRCDIR = $(realpath $(CURDIR)/src)
LOGDIR = $(OBJDIR)
MAKE_RECURSIVE = $(MAKE) -C $(OBJDIR) -f $(SRCDIR)/Makefile srcdir=$(SRCDIR) VPATH=$(SRCDIR)
OCTAVE_PATHS = --path='$(CURDIR)/inst' --path='$(OBJDIR)' --path='$(CURDIR)/tests'
OCTAVE_TEST_SCRIPT = $(CURDIR)/tests/__py_tests__.m
TARDIR = $(O)
else
OBJDIR = src
SRCDIR = $(OBJDIR)
LOGDIR = $(OBJDIR)
MAKE_RECURSIVE = $(MAKE) -C $(OBJDIR)
OCTAVE_PATHS = --path='$(CURDIR)/inst' --path='$(CURDIR)/src' --path='$(CURDIR)/tests'
OCTAVE_TEST_SCRIPT = ../tests/__py_tests__.m
TARDIR = .
endif

all:              ## build this project (default target)
clean:            ## clean all compiled object files
mostlyclean:      ## clean all intermediate object files
distclean:        ## clean all built and generated files not distributed
maintainer-clean: ## clean all built and generated files not maintained

all clean maintainer-clean mostlyclean:
	+$(MAKE_RECURSIVE) $@

distclean:
	+$(MAKE_RECURSIVE) $@
	-rm -f $(LOGDIR)/fntests.log

check: all ## run the test suite
	cd $(LOGDIR) \
	  && $(OCTAVE) --no-history --no-window-system --norc --silent \
	  $(OCTAVE_PATHS) $(OCTAVE_TEST_SCRIPT) \
	  $(shell cd inst && LC_ALL=C.UTF-8 ls *.m @*/*.m) \
	  $(shell cd tests && LC_ALL=C.UTF-8 ls *.m *.tst) \
	  $(shell cd $(OBJDIR) && LC_ALL=C.UTF-8 ls *-tst)

test: check ## synonym for check

dist: dist-gzip ## build the source distribution

dist-gzip:
	git archive --prefix=$(TARNAME)/ --output=$(TARDIR)/$(TARNAME).tar.gz HEAD

dist-zip:
	git archive --prefix=$(TARNAME)/ --output=$(TARDIR)/$(TARNAME).zip HEAD

help: ## print this make target summary
	@echo Available targets:
	@eval "$$(sed -n 's/^\([-A-Za-z]\+\):.* \+## \+\(.*\)/printf "  %-21s %s\\\\n" "\1" "\2"/p' $(MAKEFILE_LIST))"
	@echo
	@echo Optional arguments:
	@echo "  MKOCTFILE=<mkoctfile> build and link with <mkoctfile>"
	@echo "  O=<dir>               build object files in <dir>"
	@echo "  OCTAVE=<octave>       run the test suite with <octave>"
	@echo "  PYTHON=<python>       build and link with <python>"
	@echo "  PYTHON_VERSION=<n>    build and link with Python version <n>"
	@echo "  V=1                   build verbosely"
	@echo

.PHONY: all check clean dist dist-gzip dist-zip distclean help maintainer-clean mostlyclean test
