Pytave Building Instructions
============================

Pytave is built using GNU autotools.

Use the configure script to detect Octave and Boost::Python paths.

Run `./configure --help` for more info.

Dependencies
------------

Configuring:

  * a shell capable of running `configure`

Building:

  * GNU make

Linked and tested with:

  * Python 2.7
  * Boost::Python 1.58
  * Octave 4.1 (development version)

Bootstrapping from Mercurial
----------------------------

  1. `hg clone https://bitbucket.org/mtmiller/pytave`

  2. `autoreconf --install`

Building
--------

  1. `./configure`

     or

     `./configure --prefix=/directory/where/you/want/the/module/file`

  2. Run `make` to build the extension module

  3. To copy the pytave dynamic library to the prefix, run
     `make install`

Running from build directory
----------------------------

Pytave is made up of three parts. The `pytave` package, the
`pytave/pytave` module and the `pytave/_pytave` extension. When the user
uses `import pytave`, the pytave package `__init__` script loads both
the module and the extension. It is important that they both can be
found.

  1. Follow the build instructions to compile the extension.

  2. Start the Python interpreter with a Python path set to a path
     containing `_pytave.so` and the `package` directory. A common way
     to do this with the bash shell is to execute this command:

        PYTHONPATH=.libs:package python

  3. In the interpreter you can now access Octave through Python:

        import numpy
        import pytave
        pytave.feval(1, "cos", 0)
        b = pytave.addpath(".")
        pytave.feval(1, "testfile", 42)

Note that there is no `testfile.m` file in the distribution. This is
simply a demonstration of how to execute an arbitrary Octave m-file.

Running test suite in build directory
-------------------------------------

  1. Run `make check`

There are known errors in the test suite.

Creating a source dist
----------------------

  1. Run `make distcheck`

Hint about foreign systems
--------------------------

If you are running on a system incapable of executing the configure
scripts (e.g. a Microsoft Windows system), you may want to try
installing the MinGW and MSYS packages, and build everything inside that
environment. This isn't for the faint of heart though, since you
need/want to build Octave too. It is probably easier than trying to
build without configure scripts, that's all. There are no guarantees
that it will build at all, it has not been tested.
