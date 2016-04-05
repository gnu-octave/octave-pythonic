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


import numpy
import pytave
import traceback

print "No messages indicates test pass."

arr0_0 = numpy.zeros((0, 0))
arr0_1 = numpy.zeros((0, 1))
arr1_0 = numpy.zeros((1, 0))
number = numpy.array([[1.32]], numpy.float32)
arr1fT = numpy.array([[1.32], [2], [3], [4]], numpy.float32)
arr1fT2 = numpy.array([[1.32, 2, 3, 4]], numpy.float32)
arr1f = numpy.array([[1.32, 2, 3, 4]], numpy.float32)
arr1b = numpy.array([[8, 2, 3, 256]], numpy.int8)
arr1i = numpy.array([[17, 2, 3, 4]], numpy.int)
arr1i32 = numpy.array([[32, 2, 3, 4]], numpy.int32)
arr1i64 = numpy.array([[32, 2, 3, 4]], numpy.int64)
arr1a = numpy.array([[1, 2, 3, 4]])
arr2f = numpy.array([[1.32, 2, 3, 4], [5, 6, 7, 8]], numpy.float32)
arr2d = numpy.array([[1.17, 2, 3, 4], [5, 6, 7, 8]], numpy.float)
arr3f = numpy.array([[[1.32, 2, 3, 4], [5, 6, 7, 8]],
                     [[9, 10, 11, 12], [13, 14, 15, 16]]], numpy.float32)
arr1c = numpy.array([[1 + 2j, 3 + 4j, 5 + 6j, 7 + 0.5j]], numpy.complex)
arr1fc = numpy.array([[1 + 2j, 3 + 4j, 5 + 6j, 7 + 0.5j]], numpy.complex64)
arr1ch = numpy.array([["abc"]], numpy.character)
arr2ch = numpy.array([["abc"], ["def"]], numpy.character)
arr1o = numpy.array([[1.0, "abc", 2 + 3j]], numpy.object)
arr2o = numpy.array([[1.0, "abc", 2 + 3j], [4.0, arr1i, "def"]], numpy.object)

alimit_int64 = numpy.array([[-9223372036854775808L, 9223372036854775807L]],
                           numpy.int64)
alimit_int32 = numpy.array([[-2147483648, 2147483647]], numpy.int32)
alimit_int16 = numpy.array([[-32768, 32767, -32769, 32768]], numpy.int16)
alimit_int8 = numpy.array([[-128, 127, -129, 128]], numpy.int8)
alimit_uint8 = numpy.array([[0, 255, -1, 256]], numpy.uint8)


# Define a function in the Octave test session that simply returns the
# single value given to it.
pytave.eval(0, "function y = test_return (x); y = x; endfunction")

pytave.feval(1, "test_return", 1)


def equals(a, b):
    return numpy.alltrue(numpy.ravel(a == b))


def fail(msg, exc=None):
    print "FAIL:", msg
    traceback.print_stack()
    if exc is not None:
        traceback.print_exc(exc)
    print ""


def testequal(value):
    try:
        nvalue, = pytave.feval(1, "test_return", value)
        if not equals(value, nvalue):
            fail("as %s != %s" % (value, nvalue))
    except Exception, e:
        fail(value, e)


def testexpect(value, expected):
    try:
        nvalue, = pytave.feval(1, "test_return", value)
        if not equals(value, nvalue):
            fail("sent in %s, expecting %s, got %s", (value, expected, nvalue))
    except Exception, e:
        fail(value, e)


def testmatrix(value):
    try:
        nvalue, = pytave.feval(1, "test_return", value)
        class1, = pytave.feval(1, "class", value)
        class1 = class1.tostring()
        class2, = pytave.feval(1, "class", nvalue)
        class2 = class2.tostring()
        if not equals(value, nvalue):
            fail("as %s != %s" % (value, nvalue))
        if value.shape != nvalue.shape:
            fail("Size check failed for: %s. Expected shape %s, got %s  with shape %s"
                 % (value, value.shape, nvalue, nvalue.shape))
        if class1 != class2:
            fail("Type check failed for: %s. Expected %s. Got %s."
                 % (value, class1, class2))
    except Exception, e:
        fail("Execute failed: %s" % value, e)


def testobjecterror(value):
    try:
        pytave.feval(1, "test_return", value)
        print "FAIL:", (value,)
    except pytave.ObjectConvertError:
        pass
    except Exception, e:
        print "FAIL", (value,), e


def testvalueerror(*value):
    try:
        pytave.feval(1, *value)
        fail(value)
    except pytave.ValueConvertError:
        pass
    except Exception, e:
        fail(value, e)


def testparseerror(*value):
    try:
        pytave.eval(*value)
        print "FAIL:", (value,)
    except pytave.ParseError:
        pass
    except Exception, e:
        print "FAIL", (value,), e


def testvalueok(*value):
    try:
        pytave.feval(1, *value)
    except Exception, e:
        print "FAIL", (value,), e


def testevalexpect(numargout, code, expectations):
    try:
        results = pytave.eval(numargout, code)
        if not equals(results, expectations):
            fail("eval: %s : because %s != %s" % (code, results, expectations))
    except Exception, e:
        fail("eval: %s" % code, e)


def testsetget(variables, name, value):
    try:
        variables[name] = value
        if name not in variables:
            fail("set/get: %s: Should exist, not there." % name)
        result, = pytave.feval(1, "isequal", value, variables[name])
        if not result:
            fail("set/get: %s -> %s: results diverged" % (name, value))
    except Exception, e:
        fail("set/get: %s" % name, e)


def testexception(exception, func):
    try:
        func()
        fail("Expecting %s but nothing was raised." % repr(exception))
    except Exception, e:
        if not isinstance(e, exception):
            fail("Expecting %s but got %s instead" % (repr(exception), repr(e)), e)


def testlocalscope(x):

    @pytave.local_scope
    def sloppy_factorial(x):
        pytave.locals["x"] = x
        xm1, = pytave.eval(1, "x - 1")
        xm1 = xm1[0][0]
        if xm1 > 0:
            fxm1 = sloppy_factorial(xm1)
        else:
            fxm1 = 1
        pytave.locals["fxm1"] = fxm1
        fx, = pytave.eval(1, "x * fxm1")
        fx = fx[0][0]
        return fx

    try:
        fx = sloppy_factorial(x)
        fx1 = 1.0
        for k in range(1, x + 1):
            fx1 = k * fx1
        if fx != fx1:
            fail("testlocalscope: result incorrect")
    except Exception, e:
        fail("testlocalscope: %s" % (x,), e)


def objarray(obj):
    return numpy.array(obj, numpy.object)


def charray(obj):
    return numpy.array(obj, numpy.character)


testmatrix(alimit_int64)
testmatrix(alimit_int32)
testmatrix(alimit_int16)
testmatrix(alimit_int8)

# Strings

# FIXME: These tests are not working.
# testequal(["mystring"])
# testequal(["mystringåäöÅÄÖ"])

testexpect(1, numpy.array([[1]], numpy.int))
testexpect(1.0, numpy.array([[1]], numpy.float))

# Vector arrays
testmatrix(arr1a)
testmatrix(arr1f)
testmatrix(arr1fT)
testmatrix(arr1fT2)
testmatrix(arr1i)
testmatrix(arr1b)
testmatrix(arr1fc)

# 2d arrays
testmatrix(arr2f)
testmatrix(arr2d)
testmatrix(arr2ch)

# 3d arrays
testmatrix(arr3f)

# Note, both arr0_0 == arr0_0 and arr0_0 != arr0_0 are false!
if (arr0_0 != arr0_0) or (arr0_0 == arr0_0):
    print "FAIL: Zero test"

testmatrix(arr0_0)
testmatrix(arr0_1)
testmatrix(arr1_0)

# Lists
testexpect([1, 2], objarray([[1, 2]]))
testexpect([], objarray([[]]))

# Return cells with OK dimensions
testvalueok("cell", 1, 3)
testvalueok("cell", 1, 0)
testvalueok("cell", 0, 0)
testvalueok("cell", 3, 1)
testvalueok("cell", 0, 1)

# Dictionaries


# Simple dictionary tests
testexpect({"foo": 1, "bar": 2},
           {"foo": objarray([[1]]), "bar": objarray([[2]])})
# testexpect({"x": [1, 3], "y": [2, 4]},
#       {"x": objarray([[1, 3]]), "y": objarray([[2, 4]])})
# just constructing the second value with Numeric 24.2!
# testexpect({"x": [1, "baz"], "y": [2, "foobar"]},
#          {"x": objarray([[1, charray(["baz"])]]),
#       "y": objarray([[2, charray(["foobar"])]])})

testequal({"x": objarray([[arr1f]]), "y": objarray([[arr1i]])})
testequal({})
testequal({"foo": arr2o, "bar": arr2o})

# Try some invalid keys
testobjecterror({"this is not an Octave identifier": 1})
testobjecterror({1.22: 1})

# These should fail: No object conversion defined.
testobjecterror(None)
testobjecterror((1,))
testobjecterror(())

result, = pytave.feval(1, "eval", "[1, 1, 1]")
if result.shape != (1, 3):
    print "FAIL: expected length-3 vector"

result, = pytave.feval(1, "eval", "[1; 2; 3]")
if result.shape != (3, 1):
    print "FAIL: expected 3x1 matrix"

testparseerror(1, "endfunction")
testevalexpect(1, "2 + 2", (4,))
testevalexpect(1, "{2}", (objarray([[2]]),))
testevalexpect(1, "struct('foo', 2)", ({"foo": objarray([[2]])},))

testsetget(pytave.locals, "xxx", arr1f)
testsetget(pytave.globals, "xxx", arr2o)


def func():
    pytave.locals["this is not a valid Octave identifier"] = 1
testexception(pytave.VarNameError, func)


def func():
    pytave.locals["nonexistentvariable"]
testexception(KeyError, func)


def func(key):
    pytave.locals[key] = 1
testexception(TypeError, lambda: func(0.1))
testexception(TypeError, lambda: func(1))
testexception(TypeError, lambda: func([]))


def func(key):
    pytave.locals[key]
testexception(TypeError, lambda: func(0.1))
testexception(TypeError, lambda: func(1))
testexception(TypeError, lambda: func([]))

testlocalscope(5)

testexception(KeyError, lambda: pytave.locals["localvariable"])
pytave.locals["localvariable"] = 1
if "localvariable" in pytave.globals:
    fail("Local variable in globals")
del pytave.locals["localvariable"]
if "localvariable" in pytave.locals:
    fail("Could not clear local variable")
testexception(KeyError, lambda: pytave.locals["localvariable"])


def func():
    del pytave.locals["localvariable"]
testexception(KeyError, lambda: func())

testexception(KeyError, lambda: pytave.globals["globalvariable"])
pytave.globals["globalvariable"] = 1
if "globalvariable" in pytave.locals:
    fail("Global variable in locals")
del pytave.globals["globalvariable"]
if "globalvariable" in pytave.globals:
    fail("Could not clear global variable")
testexception(KeyError, lambda: pytave.globals["globalvariable"])


def func():
    del pytave.globals["globalvariable"]
testexception(KeyError, lambda: func())
