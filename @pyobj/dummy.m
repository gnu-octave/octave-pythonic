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
%% @defmethod @@pyobj dummy (@var{x})
%% Does nothing, stores doctests for now.
%%
%%
%% You can delete an object in Python and it will persist:
%% @example
%% @group
%% pyexec('d = dict(one=1, two=2)')
%% x = pyobj.fromPythonVarName('d')
%%   @result{} x =
%%       [PyObject id ...]
%%       @{'two': 2, 'one': 1@}
%%
%% % oops, overwrote d in Python:
%% pyexec('d = 42')
%%
%% % but have no fear, we still have a reference to it:
%% x
%%   @result{} x =
%%       [PyObject id ...]
%%       @{'two': 2, 'one': 1@}
%% @end group
%% @end example
%%
%% @code{pyeval} should return a @@pyobj for things it cannot convert to
%% Octave-native objects:
%% @example
%% @group
%% pyexec('import sys')             % doctest: +XFAIL
%% sysmodule = pyeval('sys')
%%   @result{} sysmodule =
%%       [PyObject id ...]
%%       <module 'sys' (built-in)>
%% @end group
%% @end example
%%
%% But it doesn't work yet, for now you have to do:
%% @example
%% @group
%% pyexec('import sys')
%% key = pyeval('sys')
%%   @result{} key = ...
%% sysmodule = pyobj(key)
%%   @result{} sysmodule =
%%       [PyObject id ...]
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
%% @seealso{pyobj}
%% @end defmethod

function dummy (x)

end
