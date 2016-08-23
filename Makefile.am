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

PY_FILES = \
  package/__init__.py \
  package/pytave.py \
  test/exceptions.py \
  test/test.py

EXTRA_DIST = $(DOC_FILES) $(OCT_SOURCE_FILES) $(PY_FILES)

EXTRA_libdir = $(PYTAVE_MODULE_INSTALL_PATH)
EXTRA_lib_LTLIBRARIES = _pytave.la

_pytave_la_CPPFLAGS = $(AM_CPPFLAGS)
_pytave_la_LDFLAGS = -module -L$(OCTAVE_LIBRARYDIR)
_pytave_la_LIBADD = libpytave.la -l$(BOOST_PYTHON_LIB) $(OCTAVE_LIBS)
_pytave_la_SOURCES = pytave.cc

noinst_LTLIBRARIES = libpytave.la
libpytave_la_CPPFLAGS = $(AM_CPPFLAGS)
libpytave_la_SOURCES = $(COMMON_SOURCE_FILES) $(PYTAVE_HEADER_FILES)

CLEANFILES = *.oct PKG_ADD PKG_DEL
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
	@echo "  * run a Python session with pytave package"
	@echo "    PYTHONPATH=\"\$$(pwd)/.libs:\$$(pwd)/package\" python"
	@echo ""

check-local:
	PYTHONPATH="$(abs_srcdir)/package:$$(pwd)/package:$$(pwd)/.libs" $(PYTHON) $(srcdir)/test/exceptions.py
	PYTHONPATH="$(abs_srcdir)/package:$$(pwd)/package:$$(pwd)/.libs" $(PYTHON) $(srcdir)/test/test.py
