#!/usr/bin/python
#
# SPDX-License-Identifier: GPL-3.0-or-later
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

"""Print the build flags for compiling and linking with a particular Python.

Usage:

  python-build-flags.py
  python-build-flags.py --get FLAG

"""

import os
import sys
import sysconfig

try:
    import argparse
except ImportError:
    import optparse  # pylint: disable=W0402


# The default OS installation prefix. If Python is installed under this prefix,
# it can be assumed that no linker flags are needed.
DEFAULT_PREFIX = "/usr"


def get_version():
    """Return the string representing the version of Python in file names."""
    pattern = "%d%d" if os.name == "nt" else "%d.%d"
    return pattern % sys.version_info[:2]


def get_python_version_abi():
    """Return the string representing the version of Python and ABI options."""
    suffix = ""
    try:
        suffix = sys.abiflags
    except AttributeError:
        pass
    return "python" + get_version() + suffix


def get_inc_dirs():
    """Return a list of directories that contain the Python header files."""
    inc_dirs = []
    basedirs = [sys.prefix]
    if sys.exec_prefix != sys.prefix:
        basedirs.append(sys.exec_prefix)
    for pfx in basedirs:
        if os.name == "nt":
            inc_dir = os.path.join(pfx, "include")
        else:
            inc_dir = os.path.join(pfx, "include", get_python_version_abi())
        inc_dirs.append(os.path.normpath(inc_dir))
    return inc_dirs


def check_python_header_file():
    """Test for the presence of required Python header files."""
    for inc_dir in get_inc_dirs():
        h_file = os.path.join(inc_dir, "Python.h")
        if os.path.isfile(h_file):
            return True
    return False


def get_preproc_opts():
    """Return a list of compiler options needed for using the Python headers."""
    return ["-I" + inc_dir for inc_dir in get_inc_dirs()]


def get_lib_dir():
    """Return the directory that contains the Python shared library."""
    if os.name == "nt":
        return os.path.join(os.path.normpath(sys.exec_prefix), "libs")
    if sys.exec_prefix != DEFAULT_PREFIX:
        return sysconfig.get_config_var("LIBDIR")
    return ""


def get_linker_opts():
    """Return a list of linker options needed for using the Python library."""
    lib_dir = get_lib_dir()
    return ["-L" + lib_dir, "-Wl,-rpath," + lib_dir] if lib_dir else []


def quotify(word):
    """Return the argument word in double quotes if necessary."""
    needs_quote = True in [ch in word for ch in r"\ "]
    return '"' + word + '"' if needs_quote else word


def get_build_flags():
    """Return a dictionary of compiler and linker options for building with Python."""
    flags = {}
    flags["CPPFLAGS"] = " ".join([quotify(opt) for opt in get_preproc_opts()])
    flags["LDFLAGS"] = " ".join([quotify(opt) for opt in get_linker_opts()])
    flags["LIBS"] = quotify("-l" + get_python_version_abi())
    return flags


def parse_args():
    """Parse the command line and return the resulting option values."""
    try:
        parser = argparse.ArgumentParser()
        parser.add_argument(
            "--check-header",
            action="store_true",
            help="check whether the Python.h header file exists",
        )
        parser.add_argument("--get", metavar="FLAG", help="get a particular build flag")
        args = parser.parse_args()
    except NameError:
        parser = optparse.OptionParser()
        parser.add_option(
            "--check-header",
            action="store_true",
            help="check whether the Python.h header file exists",
        )
        parser.add_option("--get", metavar="FLAG", help="get a particular build flag")
        options, args = parser.parse_args()
        if args:
            parser.error("unrecognized arguments: {}".format(" ".join(args)))
        args = options
    return args


def main():
    """Run the python-build-flags script."""
    flags = get_build_flags()
    args = parse_args()
    if args.check_header:
        print(check_python_header_file())
    elif args.get:
        if args.get in flags:
            print(flags[args.get])
        else:
            sys.exit(1)
    else:
        for opt in sorted(flags.keys()):
            print("{}={}".format(opt, flags[opt]))


if __name__ == "__main__":
    main()
