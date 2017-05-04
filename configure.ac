dnl Process this file with autoconf to produce a configure script.
dnl
dnl Copyright (C) 2015-2016 Mike Miller
dnl Copyright (C) 2008 David Grundberg, Håkan Fors Nilsson
dnl Copyright (C) 2009 Jaroslav Hajek, VZLU Prague
###
### This file is part of Pytave.
###
### Pytave is free software: you can redistribute it and/or modify it
### under the terms of the GNU General Public License as published by the
### Free Software Foundation, either version 3 of the License, or (at your
### option) any later version.
###
### Pytave is distributed in the hope that it will be useful, but WITHOUT
### ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
### FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
### for more details.
###
### You should have received a copy of the GNU General Public License
### along with Pytave; see the file COPYING.  If not, see
### <http://www.gnu.org/licenses/>.

AC_PREREQ(2.61)
AC_INIT(pytave, dev, [[https://bitbucket.org/mtmiller/pytave/issues]])

# Some important programs.
AC_LANG(C++)
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_CC

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([oct-py-eval.cc])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile])

# Find Octave
AX_OCTAVE([OCTAVE_CONFIG], [],
          [AC_MSG_FAILURE([unable to find Octave development files])])
pytave_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS=$OCTAVE_CPPFLAGS
CPPFLAGS=$pytave_save_CPPFLAGS

# Pick a Python library to use
AX_PYTHON_DEVEL

AM_INIT_AUTOMAKE([foreign -Wno-portability])
LT_INIT

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

AC_OUTPUT

AC_MSG_NOTICE([
========================================================================
Pytave is configured with the following setup

Dependencies
  Octave ............. $OCTAVE_INCLUDEDIR
  Python ............. $PYTHON_CPPFLAGS
    library .......... $PYTHON_LIBS
    executable ....... $PYTHON

========================================================================])
