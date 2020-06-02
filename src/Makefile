# Makefile for Octave Pythonic src directory
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

AR              ?= ar
GREP            ?= grep
MKOCTFILE       ?= mkoctfile
OCTAVE          ?= octave
PYTHON          ?= python$(PYTHON_VERSION)
PYTHON_VERSION  ?= 3
SED             ?= sed

ARFLAGS  = cr

export CXXFLAGS ?= -g -O2

srcdir = .

PYTHON_COMMAND_EXISTS := $(shell "$(PYTHON)" -c "print(True)")
PYTHON_HEADER_EXISTS  := $(shell "$(PYTHON)" $(srcdir)/../build-aux/python-build-flags.py --check-header)

ifeq ($(PYTHON_COMMAND_EXISTS),True)
ifeq ($(PYTHON_HEADER_EXISTS),True)
PYTHON_CPPFLAGS := $(shell "$(PYTHON)" $(srcdir)/../build-aux/python-build-flags.py --get CPPFLAGS)
PYTHON_LDFLAGS  := $(shell "$(PYTHON)" $(srcdir)/../build-aux/python-build-flags.py --get LDFLAGS)
PYTHON_LIBS     := $(shell "$(PYTHON)" $(srcdir)/../build-aux/python-build-flags.py --get LIBS)
else
PYTHON_CPPFLAGS = $(error The header file <Python.h> is not found, you may need to install Python header files and development libraries)
endif
else
PYTHON_CPPFLAGS = $(error The Python interpreter "$(PYTHON)" is not found or not functional, you may need to set PATH or PYTHON_VERSION)
endif

P_CPPFLAGS = -I. -I$(srcdir) $(PYTHON_CPPFLAGS)
P_CXXFLAGS = -Wall -Wextra
P_LDFLAGS  = $(PYTHON_LDFLAGS)

COMMON_SOURCES = \
  oct-py-error.cc \
  oct-py-eval.cc \
  oct-py-init.cc \
  oct-py-types.cc \
  oct-py-util.cc

COMMON_HEADERS = \
  oct-py-error.h \
  oct-py-eval.h \
  oct-py-init.h \
  oct-py-object.h \
  oct-py-types.h \
  oct-py-util.h

OCT_FILES = \
  __py_struct_from_dict__.oct \
  pycall.oct \
  pyeval.oct \
  pyexec.oct

PKG_FILES = PKG_ADD PKG_DEL

NEWS_FILE = $(srcdir)/../NEWS

COMMON_OBJECTS = $(patsubst %.cc, %.o, $(COMMON_SOURCES))
OCT_SOURCES = $(patsubst %.oct, %.cc, $(OCT_FILES))
TST_FILES = $(addsuffix -tst,$(OCT_SOURCES))

CLEANFILES = *.a *.oct *-tst $(PKG_FILES) $(NEWS_FILE)
MOSTLYCLEANFILES = *.o

OCT_COMPILE = $(MKOCTFILE) $(P_V_MKOCTFILE_FLAGS) $(P_CPPFLAGS) $(CPPFLAGS) \
  $(P_CXXFLAGS) $(CXXFLAGS) -c -o $@
OCT_LINK = $(MKOCTFILE) $(P_V_MKOCTFILE_FLAGS) $(P_CPPFLAGS) $(CPPFLAGS) \
  $(P_CXXFLAGS) $(CXXFLAGS) $(P_LDFLAGS) $(LDFLAGS) -o $@
OCT_LIBS = libpythonic.a $(PYTHON_LIBS)

P_V_AR     = $(P_V_AR_$(V))
P_V_AR_    = $(P_V_AR_0)
P_V_AR_0   = @echo "  AR    " $@;
P_V_AR_1   =
P_V_CXX    = $(P_V_CXX_$(V))
P_V_CXX_   = $(P_V_CXX_0)
P_V_CXX_0  = @echo "  CXX   " $@;
P_V_CXX_1  =
P_V_GEN    = $(P_V_GEN_$(V))
P_V_GEN_   = $(P_V_GEN_0)
P_V_GEN_0  = @echo "  GEN   " $@;
P_V_GEN_1  =
P_V_LINK   = $(P_V_LINK_$(V))
P_V_LINK_  = $(P_V_LINK_0)
P_V_LINK_0 = @echo "  LINK  " $@;
P_V_LINK_1 =
P_V_MKOCTFILE_FLAGS   = $(P_V_MKOCTFILE_FLAGS_$(V))
P_V_MKOCTFILE_FLAGS_  = $(P_V_MKOCTFILE_FLAGS_0)
P_V_MKOCTFILE_FLAGS_0 =
P_V_MKOCTFILE_FLAGS_1 = --verbose

all: $(OCT_FILES) $(PKG_FILES) $(TST_FILES) $(NEWS_FILE)

%.o: %.cc $(COMMON_HEADERS)
	$(P_V_CXX)$(OCT_COMPILE) $<

%.oct: %.o libpythonic.a $(COMMON_HEADERS)
	$(P_V_LINK)$(OCT_LINK) $< $(OCT_LIBS)

libpythonic.a: $(COMMON_OBJECTS)
	$(P_V_AR)$(AR) $(ARFLAGS) $@ $^

%.cc-tst: %.cc
	$(P_V_GEN)rm -f $@-t $@ && \
	( echo "## DO NOT EDIT!  Generated automatically from $(<F) by Make."; \
	  $(GREP) '^%!' $< \
	) > $@-t && \
	mv $@-t $@

PKG_ADD: $(OCT_SOURCES)
	$(P_V_GEN)for f in $(OCT_SOURCES); do \
	  b=$${f%.cc}; \
	  if test -f $$f; then d=.; else d=$(srcdir); fi; \
	  funcs=`$(SED) -n 's/^DEFUN.*(\(\w\+\),.*/\1/p' $$d/$$f | $(GREP) -v $$b`; \
	  if test -n "$$funcs"; then \
	    echo "$$funcs" | $(SED) "s/.*/autoload (\"&\", \"$$b.oct\");/" > $@-t && \
	    mv $@-t $@ || exit $?; \
	  fi; \
	done

PKG_DEL: $(OCT_SOURCES)
	$(P_V_GEN)for f in $(OCT_SOURCES); do \
	  b=$${f%.cc}; \
	  if test -f $$f; then d=.; else d=$(srcdir); fi; \
	  funcs=`$(SED) -n 's/^DEFUN.*(\(\w\+\),.*/\1/p' $$d/$$f | $(GREP) -v $$b`; \
	  if test -n "$$funcs"; then \
	    echo "$$funcs" | $(SED) "s/.*/autoload (\"&\", which (\"$$b.oct\"), \"remove\");/" > $@-t && \
	    mv $@-t $@ || exit $?; \
	  fi; \
	done

$(NEWS_FILE): $(NEWS_FILE).md
	$(P_V_GEN)cp $< $@

clean: mostlyclean
	-rm -f $(CLEANFILES)

distclean: clean

maintainer-clean: distclean

mostlyclean:
	-rm -f $(MOSTLYCLEANFILES)

.PHONY: all clean distclean maintainer-clean mostlyclean

.SUFFIXES: .a .cc .cc-tst .o .oct
