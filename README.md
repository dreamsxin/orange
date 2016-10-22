# Orange Mini

[![Join the chat at https://gitter.im/orange-lang/orange](https://badges.gitter.im/orange-lang/orange.svg)](https://gitter.im/orange-lang/orange?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Orange is a statically typed, multi-paradigm (imperative, object-oriented) systems programming language with a focus on high productivity and extendability.

Orange _Mini_ is a subset of Orange used to bootstrap the compiler. It lacks the following features from the full language:

- Strict aliases
- Tuples
- Short blocks
- Yield
- ADT Enums
- Switch statement
- Default values for parameters
- Named arguments 
- Implicit Generic Types
- Partial Classes
- UTF-8 support. (ASCII only in this version)
- Placeholder (`_`) variable.

# Goals

- _Be Productive_. Provide a simple syntax for writing code quickly and make it easy to download and install project dependencies. Provide strict type checking to make sure basic type errors can’t happen.
- _Be Extendible_. Allow developers to extend new interfaces to existing types, and allow for creating/downloading compiler extensions that can add new language features and code checks.
- _Be Flexible_. Combine the extendibility and productivity to make Orange suitable for any kind of development, from embedded programming to high-level applications like web servers.

# Status

Linux/OS X  | Windows
------------- | -------------
[![Build Status](https://travis-ci.org/orange-lang/orange.svg?branch=orange-mini)](https://travis-ci.org/orange-lang/orange) | [![Build status](https://ci.appveyor.com/api/projects/status/r4y46n573riuqfv1/branch/orange-mini?svg=true)](https://ci.appveyor.com/project/rfratto/orange-9no7j/branch/orange-mini)

# Resources

* [Language Guide](http://docs.orange-lang.org/v/rev-3/) ([Github](https://github.com/orange-lang/orange-docs/tree/rev-3))

# Building
Building on OS X and Linux should be straightforward. Make sure git and CMake are installed and then run the following commands:

```sh
$ git clone https://github.com/orange-lang/orange.git
$ cd orange
$ git submodule update --init
$ mkdir build-orange && cd build-orange
$ cmake ..
$ make all install
```

Building on Windows is supported through [MSYS2](https://msys2.github.io)

# Community

Orange has a [Gitter](https://gitter.im/orange-lang/orange?utm_source=share-link&utm_medium=link&utm_campaign=share-link). Come say hi! Feel free to open any issues on Github about questions, suggestions, or bugs. We also have a [Google Group](https://groups.google.com/forum/#!forum/orange-lang) open to the public.
