Octave Python Interface
=======================

This project is for development of a native Python calling interface for
[GNU Octave](http://www.octave.org).

Goals
-----

The goals of this extension include

* call any loadable Python modules, classes, and functions
* automatic translation of certain Octave data types into Python
  arguments
* hold reference to and performing operations on any Python data type as
  Octave variables
* automatic translation of certain Python data types into Octave return
  values
* be as compatible as possible with Matlab's own Python calling
  interface

Examples
--------

A few examples are listed here to give a brief introduction to how the
Python runtime is translated to Octave.

Add a directory to the Python module search path

    py.sys.path.insert (int32 (0), "/path/to/module");

Use a vectorized NumPy function

    x = py.numpy.sqrt (1:10);

Call a function with keyword arguments

    a = py.int ("5ba0", pyargs ("base", int32 (16)));

Read an entire text file into a string

    s = py.str ().join (py.open ("/etc/passwd").readlines ());

Installation
------------

There is currently no support for installing this project as an Octave
package or in a system or user directory for regular use. This is
intentional, since the project is still being developed and is not
stable enough for actual use yet.

What is supported is building and running the project from the build
directory. Building requires Octave and Python development libraries and
GNU autotools.

1. `hg clone https://bitbucket.org/mtmiller/pytave`
2. `cd pytave`
3. `autoreconf -i`
4. `./configure`
5. `make`
6. Run Octave with the build directory added to the load path

Development
-----------

We welcome all contributors, bug reports, test results, and ideas for
improvement. Contributions in any of the following forms, in no
particular order, are needed and appreciated.

* Testing on different operating systems and in different environments
* Testing for full functionality with a variety of Python libraries
* Bug reports detailing problems encountered or unexpected behavior
* Code contributions
* Documentation in the form of examples, improvements to help texts, or
  some sort of user manual

Other Resources
---------------

Please discuss or ask questions about this project on the Octave
[maintainers mailing list](https://lists.gnu.org/mailman/listinfo/octave-maintainers).

The [wiki page](http://wiki.octave.org/Python_interface) contains more
examples and ideas about the project.
