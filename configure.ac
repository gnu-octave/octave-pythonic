dnl Process this file with autoconf to produce a configure script.
dnl
dnl Copyright (C) 2015-2017 Mike Miller
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

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([oct-py-eval.cc])

AM_INIT_AUTOMAKE([foreign -Wno-portability])

AC_USE_SYSTEM_EXTENSIONS

LT_INIT

AC_PROG_CXX

AC_LANG(C++)

### Check for Octave library and mkoctfile

AX_OCTAVE([OCTAVE_CONFIG], [],
          [AC_MSG_FAILURE([unable to find Octave development files])])

### Check for Python library and interpreter

AX_PYTHON_DEVEL
for opt in $PYTHON_LIBS; do
  case "$opt" in #(
    -L/usr*) : ;; #(
    -L*) PYTHON_LIBS="-R${opt#-L} $PYTHON_LIBS" ;; #(
    *) : ;;
  esac
done

AC_OUTPUT

AC_MSG_NOTICE([

Pytave is now configured for the following

  Octave header files:  $OCTAVE_INCLUDEDIR
  Octave libraries:     $OCTAVE_LIBRARYDIR
  Python header files:  $PYTHON_CPPFLAGS
  Python library:       $PYTHON_LIBS
  Python executable:    $PYTHON
])
