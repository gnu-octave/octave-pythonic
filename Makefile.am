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
  oct-py-error.cc \
  oct-py-eval.cc \
  oct-py-init.cc \
  oct-py-types.cc \
  oct-py-util.cc

DOC_FILES = \
  CONTRIBUTORS.md \
  README.md

M_FILES = \
  @py/py.m \
  @py/subsref.m \
  @pyobject/cell.m \
  @pyobject/char.m \
  @pyobject/display.m \
  @pyobject/dummy.m \
  @pyobject/fieldnames.m \
  @pyobject/methods.m \
  @pyobject/pyobject.m \
  @pyobject/subsasgn.m \
  @pyobject/subsref.m \
  __py_tests__.m \
  pyargs.m \
  pyversion.m

OCT_FILES = \
  __py_struct_from_dict__.oct \
  pycall.oct \
  pyeval.oct \
  pyexec.oct

OCT_SOURCE_FILES = $(patsubst %.oct, %.cc, $(OCT_FILES))

COMMON_HEADER_FILES = \
  config.h \
  oct-py-error.h \
  oct-py-eval.h \
  oct-py-init.h \
  oct-py-object.h \
  oct-py-types.h \
  oct-py-util.h

TST_FILES = $(addsuffix -tst,$(OCT_SOURCE_FILES))

EXTRA_DIST = $(DOC_FILES) $(M_FILES) $(OCT_SOURCE_FILES)

noinst_LTLIBRARIES = liboctpython.la
liboctpython_la_CPPFLAGS = $(AM_CPPFLAGS)
liboctpython_la_SOURCES = $(COMMON_SOURCE_FILES) $(COMMON_HEADER_FILES)

CLEANFILES = *.oct *-tst PKG_ADD PKG_DEL fntests.log
SUFFIXES = .oct

AM_V_MKOCTFILE = $(am__v_MKOCTFILE_$(V))
am__v_MKOCTFILE_ = $(am__v_MKOCTFILE_$(AM_DEFAULT_VERBOSITY))
am__v_MKOCTFILE_0 = @echo "  MKOCTFIL" $@;
am__v_MKOCTFILE_1 =

OCT_LIBS = liboctpython.la $(PYTHON_LIBS)
OCT_LINK = $(LIBTOOL) $(AM_V_lt) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
  --mode=link $(MKOCTFILE) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
  $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@

%.oct: %.cc liboctpython.la $(COMMON_HEADER_FILES)
	$(AM_V_MKOCTFILE)$(OCT_LINK) $< $(OCT_LIBS)

%.cc-tst: %.cc
	$(AM_V_GEN)rm -f $@-t $@ && \
	( echo "## DO NOT EDIT!  Generated automatically from $(<F) by Make."; \
	  $(GREP) '^%!' $< \
	) > $@-t && \
	mv $@-t $@

PKG_ADD: $(OCT_SOURCE_FILES)
	$(AM_V_GEN)for f in $(OCT_SOURCE_FILES); do \
	  b=$${f%.cc}; \
	  if test -f $$f; then d=.; else d=$(srcdir); fi; \
	  funcs=`$(SED) -n 's/^DEFUN.*(\(\w\+\),.*/\1/p' $$d/$$f | grep -v $$b`; \
	  if test -n "$$funcs"; then \
	    echo "$$funcs" | $(SED) "s/.*/autoload (\"&\", \"$$b.oct\");/" > $@-t && \
	    mv $@-t $@ || exit $?; \
	  fi; \
	done

PKG_DEL: $(OCT_SOURCE_FILES)
	$(AM_V_GEN)for f in $(OCT_SOURCE_FILES); do \
	  b=$${f%.cc}; \
	  if test -f $$f; then d=.; else d=$(srcdir); fi; \
	  funcs=`$(SED) -n 's/^DEFUN.*(\(\w\+\),.*/\1/p' $$d/$$f | grep -v $$b`; \
	  if test -n "$$funcs"; then \
	    echo "$$funcs" | $(SED) "s/.*/autoload (\"&\", which (\"&\"), \"remove\");/" > $@-t && \
	    mv $@-t $@ || exit $?; \
	  fi; \
	done

all-local: $(OCT_FILES) PKG_ADD PKG_DEL
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
		  --path="$(abs_builddir)" --path="$(abs_srcdir)" \
		  $(srcdir)/__py_tests__.m $(M_FILES) $(TST_FILES)
