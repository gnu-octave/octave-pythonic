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
    import optparse


# The default OS installation prefix. If Python is installed under this prefix,
# it can be assumed that no linker flags are needed.
DEFAULT_PREFIX = '/usr'


def get_version():
    if os.name == 'nt':
        return '%d%d' % sys.version_info[:2]
    else:
        return '%d.%d' % sys.version_info[:2]


def get_python_version_abi():
    """Return the string representing the version of Python and ABI options."""
    suffix = ''
    try:
        suffix = sys.abiflags
    except AttributeError:
        pass
    return 'python' + get_version() + suffix


def get_inc_dirs():
    dirs = []
    basedirs = [sys.prefix]
    if sys.exec_prefix != sys.prefix:
        basedirs.append(sys.exec_prefix)
    for pfx in basedirs:
        if os.name == 'nt':
            d = os.path.join(pfx, 'include')
        else:
            d = os.path.join(pfx, 'include', get_python_version_abi())
        dirs.append(os.path.normpath(d))
    return dirs


def get_preproc_opts():
    return ['-I' + d for d in get_inc_dirs()]


def get_lib_dir():
    if os.name == 'nt':
        return os.path.join(os.path.normpath(sys.exec_prefix), 'libs')
    elif sys.exec_prefix != DEFAULT_PREFIX:
        return sysconfig.get_config_var('LIBDIR')
    else:
        return ''


def get_linker_opts():
    d = get_lib_dir()
    if d:
        return ['-L' + d, '-Wl,-rpath=' + d]
    else:
        return []


def quotify(s):
    """Return the argument in quotes if necessary."""
    if True in [ch in s for ch in r'\ ']:
        return '"' + s + '"'
    else:
        return s


def get_build_flags():
    flags = {}
    flags['CPPFLAGS'] = ' '.join([quotify(opt) for opt in get_preproc_opts()])
    flags['LDFLAGS'] = ' '.join([quotify(opt) for opt in get_linker_opts()])
    flags['LIBS'] = quotify('-l' + get_python_version_abi())
    return flags


def parse_args():
    try:
        parser = argparse.ArgumentParser()
        parser.add_argument('--get', metavar='FLAG',
                            help='get a particular build flag')
        args = parser.parse_args()
    except NameError:
        parser = optparse.OptionParser()
        parser.add_option('--get', metavar='FLAG',
                          help='get a particular build flag')
        options, args = parser.parse_args()
        if args:
            parser.error('unrecognized arguments: {}'.format(' '.join(args)))
        args = options
    return args


def main():
    """Run the python-build-flags script."""
    flags = get_build_flags()
    args = parse_args()
    if args.get:
        if args.get in flags:
            print(flags[args.get])
        else:
            sys.exit(1)
    else:
        for k in sorted(flags.keys()):
            print('{}={}'.format(k, flags[k]))


if __name__ == '__main__':
    main()
