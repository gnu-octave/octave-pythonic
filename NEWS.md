# Octave Pythonic News

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- New command `pythonic` to get information about the package.

### Fixed
- Ensure that `pyobject` constructor does not recurse or overwrite itself.
- Build with the right compiler and linker options on Windows.
- More helpful error messages when Python header files are not installed.

## 0.0.1 - 2019-05-22

This is the first release of the Octave Pythonic package.

[Unreleased]: https://gitlab.com/mtmiller/octave-pythonic/compare/v0.0.1...master
