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

"""Print the release description for a particular version.

Usage:

  release-description-gen.py 1.2.3 < NEWS.md > release-1.2.3.md

"""

import re
import sys


def extract_release_description(file, version):
    """Return the release description for version from file."""
    desc = []
    heading_match = '^## \\[?{0}\\]?( |$)'.format(re.escape(version))
    heading_match = re.compile(heading_match)
    heading_no_match = re.compile('^(## |\\[)')
    seen_match = False
    for line in file:
        if re.match(heading_match, line):
            seen_match = True
        elif seen_match and re.match(heading_no_match, line):
            break
        elif seen_match:
            desc.append(line.rstrip())

    while desc and not desc[0]:
        del desc[0]

    while desc and not desc[-1]:
        del desc[-1]

    return '\n'.join(desc)


def main():
    """Run the release-description-gen script."""
    if len(sys.argv) > 1:
        version = sys.argv[1]
        sys.stdout.write(extract_release_description(sys.stdin, version))


if __name__ == '__main__':
    main()
