#!/usr/bin/python
# -*- coding:utf-8 -*-
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

import pytave

try:
    pytave.feval(1, "")
except pytave.OctaveError as e:
    print("test ok")
except:
    print("test fail")

try:
    pytave.feval(1, "cell")
except pytave.ValueConvertError as e:
    print("test ok")
except:
    print("test fail")

try:
    pytave.feval(1, "sin", {"asdf": "asdf"})
except pytave.ObjectConvertError as e:
    print("test ok")
except:
    print("test fail")
