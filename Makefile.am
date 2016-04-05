ACLOCAL_AMFLAGS = -I m4
EXTRA_lib_LTLIBRARIES = _pytave.la
EXTRA_DIST = test/test.py test/exceptions.py \
package/__init__.py package/pytave.py
EXTRA_libdir = @PYTAVE_MODULE_INSTALL_PATH@

_pytave_la_CPPFLAGS = @OCTAVE_CPPFLAGS@ @PYTHON_CPPFLAGS@ -I@NUMPY_INCLUDEDIR@

# Having -module in LDFLAGS lets us use a non-standard library name 
# (e.g. _pytave instead of libpytave)
_pytave_la_LDFLAGS = -module -L@OCTAVE_LIBRARYDIR@ @OCTAVE_LIBS@  \
-l@BOOST_PYTHON_LIB@ -R @PYTAVE_OCTAVE_RPATH@

_pytave_la_SOURCES = octave_to_python.cc pytave.cc python_to_octave.cc \
arrayobjectdefs.h octave_to_python.h exceptions.h   \
exceptions.cc python_to_octave.h config.h

AM_V_MKOCTFILE = $(am__v_MKOCTFILE_$(V))
am__v_MKOCTFILE_ = $(am__v_MKOCTFILE_$(AM_DEFAULT_VERBOSITY))
am__v_MKOCTFILE_0 = @echo "  MKOCTFIL" $@;
am__v_MKOCTFILE_1 =

all-am: py.oct

%.oct: %.cc arrayobjectdefs.h config.h exceptions.h octave_to_python.h python_to_octave.h
	$(AM_V_MKOCTFILE)$(MKOCTFILE) $(DEFS) $(DEFAULT_INCLUDES) $(_pytave_la_CPPFLAGS) $< exceptions.cc octave_to_python.cc python_to_octave.cc -l$(BOOST_PYTHON_LIB) $(PYTHON_LIBS) -o $@

CLEANFILES = *.oct

SUFFIXES = .oct

check-local:
	PYTHONPATH="$$(pwd)/package:$$(pwd)/.libs" $(PYTHON) test/exceptions.py
	PYTHONPATH="$$(pwd)/package:$$(pwd)/.libs" $(PYTHON) test/test.py
