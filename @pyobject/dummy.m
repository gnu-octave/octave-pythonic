%% Copyright (C) 2016 Colin B. Macdonald
%%
%% This file is part of PyTave.
%%
%% OctSymPy is free software; you can redistribute it and/or modify
%% it under the terms of the GNU General Public License as published
%% by the Free Software Foundation; either version 3 of the License,
%% or (at your option) any later version.
%%
%% This software is distributed in the hope that it will be useful,
%% but WITHOUT ANY WARRANTY; without even the implied warranty
%% of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
%% the GNU General Public License for more details.
%%
%% You should have received a copy of the GNU General Public
%% License along with this software; see the file COPYING.
%% If not, see <http://www.gnu.org/licenses/>.

%% -*- texinfo -*-
%% @documentencoding UTF-8
%% @defmethod @@pyobject dummy (@var{x})
%% Does nothing, stores doctests for now.
%%
%%
%% Simple example:
%% @example
%% @group
%% pyexec('g = 6')
%% g = pyobject.fromPythonVarName('g');
%%
%% sort(whatmethods(g))
%%   @result{} ans =
%%     @{
%%       [1,1] = bit_length
%%       [1,2] = conjugate
%%       [1,3] = denominator
%%       [1,4] = imag
%%       [1,5] = numerator
%%       [1,6] = real
%%      @}
%%
%% g.numerator
%%   @result{} ans =  6
%% g.denominator
%%   @result{} ans =  1
%% @end group
%% @end example
%%
%%
%% You can delete an object in Python and it will persist:
%% @example
%% @group
%% pyexec('d = dict(one=1, two=2)')
%% x = pyobject.fromPythonVarName('d')
%%   @result{} x = [pyobject ...]
%%       @{'two': 2, 'one': 1@}
%%
%% % oops, overwrote d in Python:
%% pyexec('d = 42')
%%
%% % but have no fear, we still have a reference to it:
%% x
%%   @result{} x = [pyobject ...]
%%       @{'two': 2, 'one': 1@}
%% @end group
%% @end example
%%
%% We can accesss ``callables'' (methods) of objects:
%% @example
%% @group
%% x.keys()
%%   @result{} ans =
%%       @{
%%         [1,1] = two
%%         [1,2] = one
%%       @}
%% @end group
%% @end example
%%
%% @code{pyeval} returns a @@pyobject for things it cannot convert to
%% Octave-native objects:
%% @example
%% @group
%% pyexec('import sys')
%% sysmodule = pyeval('sys')
%%   @result{} sysmodule = [pyobject ...]
%%       <module 'sys' (built-in)>
%% @end group
%% @end example
%%
%% After you have the object, you can access its properties:
%% @example
%% @group
%% sysmodule.version
%%   @result{} ans = ...
%% @end group
%% @end example
%%
%%
%% TODO: this should return a cell array with a double, a string,
%% and an @@pyobject in it:
%% @example
%% @group
%% pyeval('[42, "hello", sys]')         % doctest: +XFAIL
%%   @result{} ans =
%%       @{
%%         [1,1] =  42
%%         [1,2] = hello
%%         [1,3] =
%%           [PyObject id ...]
%%           <module 'sys' (built-in)>
%%       @}
%% @end group
%% @end example
%%
%% @seealso{pyobject}
%% @end defmethod

function dummy (x)

end
