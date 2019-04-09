## Copyright (C) 2016 Colin B. Macdonald
## SPDX-License-Identifier: GPL-3.0-or-later
##
## This file is part of Octave Pythonic.
##
## Octave Pythonic is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## Octave Pythonic is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Octave Pythonic; see the file COPYING.  If not, see
## <https://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @documentencoding UTF-8
## @defmethod @@pyobject dummy (@var{x})
## Does nothing, stores doctests and other misc docs for now.
##
##
## Some simple Python objects are converted to equivalent Octave values:
## @example
## @group
## pyeval ("6.0")
##   @result{} ans = 6
## @end group
## @end example
##
## To ensure the return value is a @@pyobject, it can be cast:
## @example
## @group
## g = pyobject (int32 (6))
##   @result{} g = [Python object of type int]
##
##       6
##
## sort (methods (g))
##   @result{} ans =
##     @{
##       [1,1] = bit_length
##       ...
##     @}
##
## sort (fieldnames (g))
##   @result{} ans =
##     @{
##       [1,1] = denominator
##       [2,1] = imag
##       [3,1] = numerator
##       [4,1] = real
##     @}
##
## double (g.numerator)
##   @result{} ans =  6
## double (g.denominator)
##   @result{} ans =  1
## @end group
## @end example
##
##
## You can delete an object in Python and it will persist:
## @example
## @group
## pyexec ("d = dict(two=2)")
## x = pyeval ("d")
##   @result{} x = [Python object of type dict]
##       @{'two': 2@}
##
## # oops, overwrote d in Python:
## pyexec ("d = 42")
##
## # but have no fear, we still have a reference to it:
## x
##   @result{} x = [Python object of type dict]
##       @{'two': 2@}
## @end group
## @end example
##
## We can accesss ``callables'' (methods) of objects:
## @example
## @group
## double (x.pop ("two"))
##   @result{} ans =  2
## @end group
## @end example
## And note this has changed the Python dict @code{x}:
## @example
## @group
## x
##   @result{} x = [Python object of type dict]
##       @{@}
## @end group
## @end example
##
##
## @code{pyeval} returns a @@pyobject for things it cannot convert to
## Octave-native objects:
## @example
## @group
## pyexec ("import sys")
## sysmodule = pyeval ("sys")
##   @result{} sysmodule = [Python object of type module]
##       <module 'sys' (built-in)>
## @end group
## @end example
##
## After you have the object, you can access its properties:
## @example
## @group
## sysmodule.version
##   @result{} ans = ...
## @end group
## @end example
##
##
## A Python list is returned as a @@pyobject:
## @example
## @group
## L = pyeval ("[42.0, 'hello', sys]")
##   @result{} L = [Python object of type list]
##       [42.0, 'hello', <module 'sys' (built-in)>]
## @end group
## @end example
##
## Elements of the list can be accessed directly
## @example
## @group
## L@{1@}
##   @result{} ans =  42
## @end group
## @end example
## or if needed, the list can be converted to a cell array:
## @example
## @group
## @c FIXME: display of cell array in flux: https://savannah.gnu.org/bugs/?50756
## @c doctest: +XFAIL
## cell (L)
##   @result{} ans =
##       @{
##         [1,1] =  42
##         [1,2] = [Python object of type str]
##                 hello
##         [1,3] = [Python object of type module]
##                 <module 'sys' (built-in)>
##       @}
## @end group
## @end example
##
## A @@pyobject can be passed back to Python.  This does not make
## a copy but rather a reference to the original object.
## For example:
## @example
## @group
## char (pycall ("repr", sysmodule))
##   @result{} <module 'sys' (built-in)>
## @end group
## @end example
##
## @seealso{pyobject}
## @end defmethod

function dummy (x)
endfunction
