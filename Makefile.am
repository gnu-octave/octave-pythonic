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

COMMON_SOURCE_FILES = \
  exceptions.cc \
  octave_to_python.cc \
  python_to_octave.cc

OCT_FILES = \
  py.oct

PYTAVE_HEADER_FILES = \
  arrayobjectdefs.h \
  config.h \
  exceptions.h \
  octave_to_python.h \
  python_to_octave.h

PY_FILES = \
  package/__init__.py \
  package/pytave.py \
  test/exceptions.py \
  test/test.py

EXTRA_DIST = $(PY_FILES)

EXTRA_libdir = $(PYTAVE_MODULE_INSTALL_PATH)
EXTRA_lib_LTLIBRARIES = _pytave.la

_pytave_la_CPPFLAGS = $(AM_CPPFLAGS)
_pytave_la_LDFLAGS = -module -L$(OCTAVE_LIBRARYDIR)
_pytave_la_LIBADD = -l$(BOOST_PYTHON_LIB) $(OCTAVE_LIBS)
_pytave_la_SOURCES = pytave.cc $(COMMON_SOURCE_FILES) $(PYTAVE_HEADER_FILES)

CLEANFILES = *.oct
SUFFIXES = .oct

AM_V_MKOCTFILE = $(am__v_MKOCTFILE_$(V))
am__v_MKOCTFILE_ = $(am__v_MKOCTFILE_$(AM_DEFAULT_VERBOSITY))
am__v_MKOCTFILE_0 = @echo "  MKOCTFIL" $@;
am__v_MKOCTFILE_1 =

OCT_LIBS = -l$(BOOST_PYTHON_LIB) $(PYTHON_LIBS)
OCT_LINK = $(MKOCTFILE) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
  $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@

%.oct: %.cc $(PYTAVE_HEADER_FILES)
	$(AM_V_MKOCTFILE)$(OCT_LINK) $< $(COMMON_SOURCE_FILES) $(OCT_LIBS)

all-local: $(OCT_FILES)

check-local:
	PYTHONPATH="$$(pwd)/package:$$(pwd)/.libs" $(PYTHON) test/exceptions.py
	PYTHONPATH="$$(pwd)/package:$$(pwd)/.libs" $(PYTHON) test/test.py
