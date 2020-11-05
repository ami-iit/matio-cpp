# matio-cpp <a href="https://isocpp.org"><img src="https://img.shields.io/badge/standard-C++14-blue.svg?style=flat&logo=c%2B%2B" alt="C++ Standard" /></a> 

**``matio-cpp``** is a C++ wrapper for the [``matio``](https://github.com/tbeu/matio) library, automatically dealing with memory allocation and deallocation.

# Overview

- [Installation](#installation)
- [Inclusion](#inclusion)
- [Supported Data Types](#supported-data-types)
- [Example of usage](#example-of-usage)
- [Known limitations](#known-limitations)
- [Similar projects](#similar-projects)

# Installation
## Dependencies

The depencies are [``CMake``](https://cmake.org/) (minimum version 3.10) and [``matio``](https://github.com/tbeu/matio) . While we suggest to follow the build instructions provided in the [``matio`` home page](https://github.com/tbeu/matio), it can also installed from common package managers:
- Linux: ``sudo apt install libmatio-dev``
- macOS: ``brew install libmatio``
- Windows, via [``vcpkg``](https://github.com/microsoft/vcpkg): ``vcpkg install --triplet x64-windows matio``
For running the tests, it is necessary to install [`Catch2`](https://github.com/catchorg/Catch2). Where supported, [``valgrind``](https://valgrind.org/) can be installed to check for memory leaks.

## Linux/macOS
```sh
git clone https://github.com/dic-iit/matio-cpp
cd matio-cpp
mkdir build && cd build
cmake ../
make
[sudo] make install
```
Notice: `sudo` is not necessary if you specify the `CMAKE_INSTALL_PREFIX`. In this case it is necessary to add in the `.bashrc` or `.bash_profile` the following lines:
```sh
export matioCpp_DIR=/path/where/you/installed/
```
## Windows
With IDE build tool facilities, such as Visual Studio:
```sh
git clone https://github.com/dic-iit/matio-cpp
cd matio-cpp
mkdir build && cd build
cmake ..
cmake --build . --target ALL_BUILD --config Release
cmake --build . --target INSTALL --config Release
```

In order to allow CMake finding ``matio-cpp``, it is necessary that the installation folder is in the ``PATH``.

# Inclusion

  ``matio-cpp`` provides native CMake support which allows the library to be easily used in CMake projects

  In order to use ``matio-cpp`` in your project, add the following in your ``CMakeLists.txt``
```cmake
find_package(matioCpp REQUIRED)
.
.
.
target_link_libraries(yourTarget matioCpp::matioCpp)
```

# Supported Data Types
``matio-cpp`` can handle the following data types:
- ``Element``, like ``double``, ``int``, ...
-  ``String``
-  ``Vector``, i.e. 1-dimensional arrays of primitive types
-  ``MultiDimensionalArray``, i.e. n-dimensional arrays of primitive types
-  ``CellArray``,  i.e. n-dimensional arrays of generic types
-  ``Struct``, i.e. a collection of name/variable pairs
-  ``StructArray``, i.e. n-dimensional arrays of ``Structs``

All these types can be read/written from/to ``.mat`` files.

# Example of usage

Read a ``.mat`` file
```c++
#include <matioCpp/matioCpp.h>

// ...

matioCpp::File input("input.mat");
// You can check if input is open with the isOpen() method
matioCpp::CellArray cellArray = input.read("cell_array").asCellArray(); //Read a Cell Array named "cell_array"
matioCpp::Element<double> doubleVar = cellMatrix({1,2,3}).asElement<double>(); //Get the element in the cell array at position (1,2,3) (0-based), casting it as a double Element
doubleVar = 3.14; //Set the doubleVar to a new value
assert(cellMatrix({1,2,3}).asElement<double>()() == 3.14); //Also the original cell array is modified, but not in the file.

```

Write a ``.mat`` file
```c++
#include <matioCpp/matioCpp.h>

// ...

matioCpp::File file = matioCpp::File::Create("test.mat"); //If the file already exists, use the same cnstructor as the example above

std::vector<double> in = {2.0,4.0,6.0,8.0};
matioCpp::Vector<int> out("test_vector");
out = in;
file.write(out);

matioCpp::String testString("string_name");
testString = "string content";
file.write(testString);

```

# Known Limitations
 - Boolean arrays are not yet supported
 - Complex arrays are not yet supported
 - Cannot read timeseries from a ``.mat`` file (this is a ``matio`` limitation https://github.com/tbeu/matio/issues/99)
 - Cannot read string arrays from a ``.mat`` file (this is a ``matio`` limitation https://github.com/tbeu/matio/issues/98)
 - Cannot read strings in a ``Struct`` from a ``.mat`` file (this is a ``matio`` limitation related to https://github.com/tbeu/matio/issues/98)

# Similar Projects
- [``eigen-matio``](https://github.com/tesch1/eigen-matio)
- [``matiopp``](https://github.com/bmc-labs/matiopp)
- [``matiocpp``](https://github.com/joka90/matiocpp)
- [``matiocpp``(2)](https://github.com/ldobinson/matiocpp)
