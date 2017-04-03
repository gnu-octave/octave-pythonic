# Makefile for Pytave
#
# Copyright (C) 2015-2016 Mike Miller
# Copyright (C) 2008 David Grundberg, Håkan Fors Nilsson
# Copyright (C) 2009 Jaroslav Hajek, VZLU Prague
#
# This file is part of Pytave.
#
# Pytave is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation, either version 3 of the License, or (at your
# option) any later version.
#
# Pytave is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License
# along with Pytave; see the file COPYING.  If not, see
# <http://www.gnu.org/licenses/>.

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = $(OCTAVE_CPPFLAGS) $(PYTHON_CPPFLAGS) -I$(NUMPY_INCLUDEDIR)

INCFLAGS = $(DEFAULT_INCLUDES)
export INCFLAGS

COMMON_SOURCE_FILES = \
  exceptions.cc \
  octave_to_python.cc \
  oct-py-eval.cc \
  oct-py-types.cc \
  oct-py-util.cc \
  python_to_octave.cc

DOC_FILES = \
  INSTALL.md \
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

PYTAVE_HEADER_FILES = \
  arrayobjectdefs.h \
  config.h \
  exceptions.h \
  oct-py-eval.h \
  oct-py-types.h \
  oct-py-util.h \
  octave_to_python.h \
  python_to_octave.h

TST_FILES = $(addsuffix -tst,$(OCT_SOURCE_FILES))

EXTRA_DIST = $(DOC_FILES) $(OCT_SOURCE_FILES)

EXTRA_libdir = $(PYTAVE_MODULE_INSTALL_PATH)
EXTRA_lib_LTLIBRARIES = _pytave.la

_pytave_la_CPPFLAGS = $(AM_CPPFLAGS)
_pytave_la_LDFLAGS = -module -L$(OCTAVE_LIBRARYDIR)
_pytave_la_LIBADD = libpytave.la -l$(BOOST_PYTHON_LIB) $(OCTAVE_LIBS)
_pytave_la_SOURCES = pytave.cc

noinst_LTLIBRARIES = libpytave.la
libpytave_la_CPPFLAGS = $(AM_CPPFLAGS)
libpytave_la_SOURCES = $(COMMON_SOURCE_FILES) $(PYTAVE_HEADER_FILES)

CLEANFILES = *.oct *-tst PKG_ADD PKG_DEL fntests.log
SUFFIXES = .oct

AM_V_MKOCTFILE = $(am__v_MKOCTFILE_$(V))
am__v_MKOCTFILE_ = $(am__v_MKOCTFILE_$(AM_DEFAULT_VERBOSITY))
am__v_MKOCTFILE_0 = @echo "  MKOCTFIL" $@;
am__v_MKOCTFILE_1 =

OCT_LIBS = libpytave.la -l$(BOOST_PYTHON_LIB) $(PYTHON_LIBS)
OCT_LINK = $(LIBTOOL) $(AM_V_lt) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
  --mode=link $(MKOCTFILE) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
  $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@

%.oct: %.cc libpytave.la $(PYTAVE_HEADER_FILES)
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
	  funcs=`$(SED) -n 's/^DEFUN.*(\(\w\+\),.*/\1/p' $$f | grep -v $$b`; \
	  if test -n "$$funcs"; then \
	    echo "$$funcs" | $(SED) "s/.*/autoload (\"&\", \"$$b.oct\");/" > $@-t && \
	    mv $@-t $@ || exit $?; \
	  fi; \
	done

PKG_DEL: $(OCT_SOURCE_FILES)
	$(AM_V_GEN)for f in $(OCT_SOURCE_FILES); do \
	  b=$${f%.cc}; \
	  funcs=`$(SED) -n 's/^DEFUN.*(\(\w\+\),.*/\1/p' $$f | grep -v $$b`; \
	  if test -n "$$funcs"; then \
	    echo "$$funcs" | $(SED) "s/.*/autoload (\"&\", which (\"&\"), \"remove\");/" > $@-t && \
	    mv $@-t $@ || exit $?; \
	  fi; \
	done

all-local: $(OCT_FILES) PKG_ADD PKG_DEL
	@echo ""
	@echo "Pytave successfully built.  Now choose from the following:"
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
		  __py_tests__.m $(M_FILES) $(TST_FILES)
