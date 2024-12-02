# matio-cpp

``matio-cpp`` is a C++ wrapper for the [``matio``](https://github.com/tbeu/matio) library, automatically dealing with memory allocation and deallocation.
It can be used for reading and writing binary MATLAB `.mat` files from C++, without the need to access or rely on MATLAB's own shared libraries.

# Overview

- [Installation](@ref installation)
- [Inclusion](@ref inclusion)
- [Supported Data Types](@ref supported-data-types)
- [Example of usage](@ref example-of-usage)
- [Example](@ref example)
- [Known limitations](@ref known-limitations)
- [Similar projects](@ref similar-projects)

# Installation{#installation}
## Dependencies

The depencies are [``CMake``](https://cmake.org/) (minimum version 3.10) and [``matio``](https://github.com/tbeu/matio). While we suggest to follow the build instructions provided in the [``matio`` home page](https://github.com/tbeu/matio), it can also installed from common package managers:
- Linux: ``sudo apt install libmatio-dev``
- Linux, macOS, Windows, via [``conda-forge``](https://conda-forge.org/): ``mamba install -c conda-forge libmatio``

[`Eigen`](https://eigen.tuxfamily.org/index.php) is an optional dependency. If available, some conversions are defined.

For running the tests, it is necessary to install [`Catch2`](https://github.com/catchorg/Catch2). Where supported, [``valgrind``](https://valgrind.org/) can be installed to check for memory leaks.

## Linux/macOS
```sh
git clone https://github.com/ami-iit/matio-cpp
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
git clone https://github.com/ami-iit/matio-cpp
cd matio-cpp
mkdir build && cd build
cmake ..
cmake --build . --target ALL_BUILD --config Release
cmake --build . --target INSTALL --config Release
```

In order to allow CMake finding ``matio-cpp``, it is necessary that the installation folder is in the ``PATH``.

# Inclusion{#inclusion}

  ``matio-cpp`` provides native CMake support which allows the library to be easily used in CMake projects

  In order to use ``matio-cpp`` in your project, add the following in your ``CMakeLists.txt``
```cmake
find_package(matioCpp REQUIRED)

# ...

target_link_libraries(yourTarget PRIVATE matioCpp::matioCpp)
```

# Supported Data Types{#supported-data-types}
``matio-cpp`` can handle the following data types:
- [Element](@ref matioCpp::Element), like ``double``, ``int``, ...
-  [String](@ref matioCpp::String)
-  [Vector](@ref matioCpp::Vector), i.e. 1-dimensional arrays of primitive types
-  [MultiDimensionalArray](@ref matioCpp::MultiDimensionalArray) , i.e. n-dimensional arrays of primitive types
-  [CellArray](@ref matioCpp::CellArray) ,  i.e. n-dimensional arrays of generic types
-  [Struct](@ref matioCpp::Struct), i.e. a collection of name/variable pairs
-  [StructArray](@ref matioCpp::StructArray), i.e. n-dimensional arrays of ``Structs``

All these types can be read/written from/to ``.mat`` files.

# Example of usage{#example-of-usage}

Read a ``.mat`` file
```{cpp}
#include <matioCpp/matioCpp.h>

// ...

matioCpp::File input("input.mat");
// You can check if input is open with the isOpen() method
matioCpp::CellArray cellArray = input.read("cell_array").asCellArray(); //Read a Cell Array named "cell_array"
matioCpp::Element<double> doubleVar = cellArray({1,2,3}).asElement<double>(); //Get the element in the cell array at position (1,2,3) (0-based), casting it as a double Element
doubleVar = 3.14; //Set the doubleVar to a new value
assert(cellArray({1,2,3}).asElement<double>()() == 3.14); //Also the original cell array is modified, but not in the file.

```

Write a ``.mat`` file
```{cpp}
#include <matioCpp/matioCpp.h>

// ...

matioCpp::File file = matioCpp::File::Create("test.mat"); //If the file already exists, use the same cnstructor as the example above

std::vector<double> in = {2.0,4.0,6.0,8.0};
matioCpp::Vector<double> out("test_vector");
out = in;
file.write(out);

matioCpp::String testString("string_name");
testString = "string content";
file.write(testString);

```

It is possibile to convert common types to ``matioCpp`` types with the function ``matioCpp::make_variable``. Examples:
```{cpp}
std::vector<double> stdVec = {1.0, 2.0, 3.0, 4.0, 5.0};
auto toMatioVec = matioCpp::make_variable("test", stdVec);

std::array<float,3> array = {1.0, 2.0, 3.0};
auto toMatioArray = matioCpp::make_variable("test", array);

int classicalArray[] = {1, 2, 3};
auto toMatioClassic = matioCpp::make_variable("test", matioCpp::make_span(classicalArray, 3));

std::string string("something");
auto toMatioString = matioCpp::make_variable("name", string);

std::vector<bool> vecOfBool = {true, false, true};
auto toVecofBool = matioCpp::make_variable("vecOfBool", vecOfBool);

auto matioDouble = matioCpp::make_variable("double", 5.0);

auto matioBool = matioCpp::make_variable("bool", true);

auto matioInt = matioCpp::make_variable("int", 2);

auto matioChar = matioCpp::make_variable("char", 'f');

std::vector<std::string> stringVector = {"Huey", "Dewey", "Louie"};
auto matioCell = matioCpp::make_variable("stringVector", stringVector);
```
If ``eigen`` is available, it is also possible to convert from and to ``eigen`` types:
```{cpp}
matioCpp::Vector<double> vector("vector", 5);                                               
Eigen::VectorXd eigenVec = matioCpp::to_eigen(vector);                                      
matioCpp::MultiDimensionalArray<float> matioCppMatrix("matrix"); 

Eigen::MatrixXf toEigenMatrix = matioCpp::to_eigen(matioCppMatrix);

Eigen::Matrix3f eigenMatrix;                                                    
eigenMatrix << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;                     
auto toMatioMatrix = matioCpp::make_variable("testMatrix", eigenMatrix);        
Eigen::Vector3i eigenVec;      
eigenVec << 2, 4, 6;                                                            
auto toMatioEigenVec = matioCpp::make_variable("testEigen", eigenVec);          
```

``matioCpp`` also exploits [``visit_struct``](https://github.com/garbageslam/visit_struct) to parse C++ structs into ``matioCpp`` structs. Example:
```{cpp}
struct testStruct
{
    int i{1};
    double d{2.0};
    std::string s{"test"};
    std::vector<double> stdVec = {1.0, 2.0, 3.0, 4.0, 5.0};
    int* notSupported = nullptr;
    std::vector<std::string> stringVector = {"Huey", "Dewey", "Louie"};
    std::vector<bool> vecOfBool = {true, false, true};
};
VISITABLE_STRUCT(testStruct, i, d, s, stdVec, vecOfBool, stringVector);

//----------

testStruct s;
matioCpp::Struct automaticStruct = matioCpp::make_variable("testStruct", s);
```

# Example{#example}
You can check the example in the ``example`` folder on how to include and use ``matioCpp``.

# Known Limitations{#known-limitations}
 - Complex arrays are not yet supported
 - Cannot read timeseries from a ``.mat`` file (this is a ``matio`` limitation https://github.com/tbeu/matio/issues/99)
 - Cannot read string arrays from a ``.mat`` file (this is a ``matio`` limitation https://github.com/tbeu/matio/issues/98)
 - Cannot read strings in a ``Struct`` from a ``.mat`` file (this is a ``matio`` limitation related to https://github.com/tbeu/matio/issues/98)

# Similar Projects{#similar-projects}
- [``eigen-matio``](https://github.com/tesch1/eigen-matio)
- [``matiopp``](https://github.com/bmc-labs/matiopp)
- [``matiocpp``](https://github.com/joka90/matiocpp)
- [``matiocpp``(2)](https://github.com/ldobinson/matiocpp)
