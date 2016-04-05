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

# Load our custom m4 scripts (from the m4 directory)
AC_CONFIG_MACRO_DIR(m4)
AC_CONFIG_SRCDIR([pytave.cc])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile])

dnl Define a convenience m4 macro for appending to pytave_missing_libs
m4_define([addmissing], [pytave_missing_libs="$1
$pytave_missing_libs"])
pytave_missing_libs=

# Find Octave
AX_OCTAVE([OCTAVE_CONFIG], [], [addmissing([Octave development files])])

# Pick a Python library to use
AX_PYTHON_DEVEL

# Look for numpy headers
AX_PYTHON_NUMPY([], [addmissing([NumPy development files])])

# Look for boost::python
AX_BOOST_PYTHON
if test -z "$BOOST_PYTHON_LIB"; then
  addmissing([Boost Python development files])
fi

AM_INIT_AUTOMAKE([-Wno-portability])
LT_INIT

# Checks for header files.
AC_CHECK_HEADERS([locale.h])

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

# Checks for library functions.
AC_CHECK_FUNCS([uselocale], [pytave_have_uselocale=yes],
                            [pytave_have_uselocale=no])

# This needs a more usable, less unusual solution.
AS_IF([test "x${prefix}" == "xNONE"],
      [PYTAVE_MODULE_INSTALL_PATH=/../invalidpath],
      [PYTAVE_MODULE_INSTALL_PATH=${prefix}])
AC_SUBST(PYTAVE_MODULE_INSTALL_PATH)

AS_IF([test -n "$pytave_missing_libs"],
      [AC_MSG_ERROR([One or more library dependencies could not be resolved.

Missing dependencies:
$pytave_missing_libs
])])

AC_OUTPUT

AC_MSG_NOTICE([
========================================================================
Pytave is configured with the following setup

Dependencies
  Octave ............. $OCTAVE_INCLUDEDIR
  Python ............. $PYTHON_CPPFLAGS
    executable ....... $PYTHON
  Python boost ....... $BOOST_PYTHON_LIB

Features
  uselocale .......... $pytave_have_uselocale

========================================================================])

AS_IF([test "x$pytave_have_uselocale" = "xno"],
      [AC_MSG_WARN([This system doesn't have uselocale support.
           The Octave interpreter will NOT work correctly in any
           locale but the C (POSIX) locale.  Pytave would use the
           uselocale function to work around this limitation.  Pytave
           can be compiled without it but will only work correcly in C
           (POSIX) locale.])])
