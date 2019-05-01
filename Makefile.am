# Makefile for Octave Pythonic
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright (C) 2015-2019 Mike Miller
# Copyright (C) 2008 David Grundberg, Håkan Fors Nilsson
# Copyright (C) 2009 Jaroslav Hajek, VZLU Prague
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

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = $(OCTAVE_CPPFLAGS) $(PYTHON_CPPFLAGS)
AM_CXXFLAGS = -W -Wall

INCFLAGS = $(DEFAULT_INCLUDES)
export INCFLAGS

COMMON_SOURCE_FILES = \
  src/oct-py-error.cc \
  src/oct-py-eval.cc \
  src/oct-py-init.cc \
  src/oct-py-types.cc \
  src/oct-py-util.cc

DOC_FILES = \
  CONTRIBUTORS.md \
  README.md

M_FILES = \
  inst/@py/py.m \
  inst/@py/subsref.m \
  inst/@pyobject/cell.m \
  inst/@pyobject/char.m \
  inst/@pyobject/display.m \
  inst/@pyobject/dummy.m \
  inst/@pyobject/fieldnames.m \
  inst/@pyobject/methods.m \
  inst/@pyobject/pyobject.m \
  inst/@pyobject/subsasgn.m \
  inst/@pyobject/subsref.m \
  inst/pyargs.m \
  inst/pyversion.m \
  tests/__py_tests__.m

OCT_FILES = \
  src/__py_struct_from_dict__.oct \
  src/pycall.oct \
  src/pyeval.oct \
  src/pyexec.oct

OCT_SOURCE_FILES = $(patsubst %.oct, %.cc, $(OCT_FILES))

COMMON_HEADER_FILES = \
  config.h \
  src/oct-py-error.h \
  src/oct-py-eval.h \
  src/oct-py-init.h \
  src/oct-py-object.h \
  src/oct-py-types.h \
  src/oct-py-util.h

TST_FILES = $(addsuffix -tst,$(OCT_SOURCE_FILES))

EXTRA_DIST = $(DOC_FILES) $(M_FILES) $(OCT_SOURCE_FILES)

noinst_LTLIBRARIES = src/liboctpython.la
src_liboctpython_la_CPPFLAGS = $(AM_CPPFLAGS)
src_liboctpython_la_SOURCES = $(COMMON_SOURCE_FILES) $(COMMON_HEADER_FILES)

CLEANFILES = src/*.oct src/*-tst src/PKG_ADD src/PKG_DEL fntests.log
SUFFIXES = .oct

AM_V_MKOCTFILE = $(am__v_MKOCTFILE_$(V))
am__v_MKOCTFILE_ = $(am__v_MKOCTFILE_$(AM_DEFAULT_VERBOSITY))
am__v_MKOCTFILE_0 = @echo "  MKOCTFIL" $@;
am__v_MKOCTFILE_1 =

OCT_LIBS = src/liboctpython.la $(PYTHON_LIBS)
OCT_LINK = $(LIBTOOL) $(AM_V_lt) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
  --mode=link $(MKOCTFILE) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
  $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@

%.oct: %.cc src/liboctpython.la $(COMMON_HEADER_FILES)
	$(AM_V_MKOCTFILE)$(OCT_LINK) $< $(OCT_LIBS)

%.cc-tst: %.cc
	$(AM_V_GEN)rm -f $@-t $@ && \
	( echo "## DO NOT EDIT!  Generated automatically from $(<F) by Make."; \
	  $(GREP) '^%!' $< \
	) > $@-t && \
	mv $@-t $@

src/PKG_ADD: $(OCT_SOURCE_FILES)
	$(AM_V_GEN)for f in $(OCT_SOURCE_FILES); do \
	  b=$${f%.cc}; b=$${b##*/}; \
	  if test -f $$f; then d=.; else d=$(srcdir); fi; \
	  funcs=`$(SED) -n 's/^DEFUN.*(\(\w\+\),.*/\1/p' $$d/$$f | grep -v $$b`; \
	  if test -n "$$funcs"; then \
	    echo "$$funcs" | $(SED) "s/.*/autoload (\"&\", \"$$b.oct\");/" > $@-t && \
	    mv $@-t $@ || exit $?; \
	  fi; \
	done

src/PKG_DEL: $(OCT_SOURCE_FILES)
	$(AM_V_GEN)for f in $(OCT_SOURCE_FILES); do \
	  b=$${f%.cc}; b=$${b##*/}; \
	  if test -f $$f; then d=.; else d=$(srcdir); fi; \
	  funcs=`$(SED) -n 's/^DEFUN.*(\(\w\+\),.*/\1/p' $$d/$$f | grep -v $$b`; \
	  if test -n "$$funcs"; then \
	    echo "$$funcs" | $(SED) "s/.*/autoload (\"&\", which (\"&\"), \"remove\");/" > $@-t && \
	    mv $@-t $@ || exit $?; \
	  fi; \
	done

all-local: $(OCT_FILES) src/PKG_ADD src/PKG_DEL
	@echo ""
	@echo "Pythonic successfully built.  Now choose from the following:"
	@echo ""
	@echo "  * run the test suite"
	@echo "    make check"
	@echo ""
	@echo "  * run an Octave session with py* functions"
	@echo "    octave --path=\"\$$(pwd)\""
	@echo ""

check-local: $(M_FILES) $(TST_FILES)
	$(OCTAVE) --no-gui-libs --no-history --no-window-system --norc \
		  --path="$(abs_builddir)/src" --path="$(abs_srcdir)" --path="$(abs_srcdir)/inst" --path="$(abs_srcdir)/tests" \
		  $(srcdir)/tests/__py_tests__.m $(M_FILES) $(TST_FILES)
