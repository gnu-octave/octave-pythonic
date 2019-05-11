Octave Pythonic Package
=======================

Pythonic is a package that provides a [Python](https://www.python.org/) native
calling interface for [GNU Octave](https://www.octave.org).

## Usage

Install this package in Octave using the `pkg` command

    pkg install https://gitlab.com/mtmiller/octave-pythonic/-/archive/master/octave-pythonic-master.tar.gz

or download and install in the command shell

    curl -O https://gitlab.com/mtmiller/octave-pythonic/-/archive/master/octave-pythonic-master.tar.gz
    octave --eval "pkg install octave-pythonic-master.tar.gz"

Load the package before using it

    pkg load pythonic

Once the package is loaded, any available Python class or function can be
invoked with the `py` package prefix. A few very basic examples to get started
are listed here.

- Add a directory to the Python module search path

      py.sys.path.insert (int32 (0), "/path/to/module");

- Use a vectorized NumPy function

      x = py.numpy.sqrt (1:10);

- Call a function with keyword arguments

      a = py.int ("5ba0", pyargs ("base", int32 (16)));

- Read an entire text file into a string

      s = py.str ().join (py.open ("/etc/motd").readlines ());

## Python Selection

The specific Python used by the package is selected when the package is
installed. It is compiled in to the installed package and cannot currently be
changed dynamically. To check which Python is used, use the `pyversion`
command

    >> pkg load pythonic
    >> pyversion
           version: "3.7"
        executable: "/usr/bin/python3"
           library: "/usr/lib/x86_64-linux-gnu/libpython3.7m.so"
            prefix: "/usr"
            loaded: 1

The default is to use the `python` program found by a standard path search.
Use the `PYTHON` or `PYTHON_VERSION` environment variables to override the
default. For example

    >> setenv PYTHON_VERSION 3
    >> pkg install …

or

    >> setenv PYTHON /home/user/anaconda3/bin/python
    >> pkg install …

## Goals

The goals of this package include

- call any loadable Python modules, classes, and functions
- hold reference to and operate on any Python data type as Octave variables
- translate certain Octave data types into Python arguments automatically
- translate certain Python data types into Octave return values automatically
- be as compatible as possible with Matlab's Python calling interface

## Development

This package also supports building, using, and testing directly from the code
repository.

    git clone https://gitlab.com/mtmiller/octave-pythonic.git
    cd octave-pythonic
    make
    make check
    octave --path $PWD/inst --path $PWD/src

The build system can be configured to use a separate object directory, for
example to build with two different versions of Python

    mkdir py2 py3
    make O=py2 PYTHON_VERSION=2
    make O=py3 PYTHON_VERSION=3

## Get Involved

We welcome all contributors, bug reports, test results, and ideas for
improvement. Contributions in any of the following forms, in no
particular order, are needed and appreciated.

- Testing on different operating systems and in different environments
- Testing for full functionality with a variety of Python libraries
- Bug reports detailing problems encountered or unexpected behavior
- Code contributions
- Documentation in the form of examples, improvements to help texts, or some
  sort of user manual

## Other Resources

Please discuss or ask questions about this package on the
[Octave maintainers mailing list](https://lists.gnu.org/mailman/listinfo/octave-maintainers).

The [wiki page](https://wiki.octave.org/Python_interface) contains more
examples and ideas about this package.
