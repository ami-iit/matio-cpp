# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia . All rights reserved.
# This software may be modified and distributed under the terms of the
# BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).

cmake_minimum_required(VERSION 3.10)
project(matioCpp VERSION 0.3.0 LANGUAGES CXX)

# Defines the CMAKE_INSTALL_LIBDIR, CMAKE_INSTALL_BINDIR and many other useful macros.
# See https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html
include(GNUInstallDirs)

# Control where libraries and executables are placed during the build.
# With the following settings executables are placed in <the top level of the
# build tree>/bin and libraries/archives in <top level of the build tree>/lib.
# This is particularly useful to run ctests on libraries built on Windows
# machines: tests, which are executables, are placed in the same folders of
# dlls, which are treated as executables as well, so that they can properly
# find the libraries to run. This is a because of missing RPATH on Windows.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

# To build shared libraries in Windows, we set CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS to TRUE.
# See https://cmake.org/cmake/help/v3.4/variable/CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS.html
# See https://blog.kitware.com/create-dlls-on-windows-without-declspec-using-new-cmake-export-all-feature/
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Under MSVC, we set CMAKE_DEBUG_POSTFIX to "d" to add a trailing "d" to library
# built in debug mode. In this Windows user can compile, build and install the
# library in both Release and Debug configuration avoiding naming clashes in the
# installation directories.
if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")

    option(DISABLE_PERMISSIVE "Add the /permissive- flag to MSVC" OFF)
    mark_as_advanced(DISABLE_PERMISSIVE)
endif()

# Build position independent code.
# Position Independent Code (PIC) is commonly used for shared libraries so that
# the same shared library code can be loaded in each program address space in a
# location where it will not overlap with any other uses of such memory.
# In particular, this option avoids problems occurring when a process wants to
# load more than one shared library at the same virtual address.
# Since shared libraries cannot predict where other shared libraries could be
# loaded, this is an unavoidable problem with the traditional shared library
# concept.
# Generating position-independent code is often the default behavior for most
# modern compilers.
# Moreover linking a static library that is not built with PIC from a shared
# library will fail on some compiler/architecture combinations.
# Further details on PIC can be found here:
# https://eli.thegreenplace.net/2011/11/03/position-independent-code-pic-in-shared-libraries/
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Disable C and C++ compiler extensions.
# C/CXX_EXTENSIONS are ON by default to allow the compilers to use extended
# variants of the C/CXX language.
# However, this could expose cross-platform bugs in user code or in the headers
# of third-party dependencies and thus it is strongly suggested to turn
# extensions off.
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

### Options
# Shared/Dynamic or Static library?
option(BUILD_SHARED_LIBS "Build libraries as shared as opposed to static" ON)


# Build test related commands?
option(BUILD_TESTING "Create tests using CMake" OFF)
if(BUILD_TESTING)
    enable_testing()
endif()

# Enable RPATH support for installed binaries and libraries
include(AddInstallRPATHSupport)
add_install_rpath_support(BIN_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"
                          LIB_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"
                          INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
                          USE_LINK_PATH)

# Encourage user to specify a build type (e.g. Release, Debug, etc.), otherwise set it to Release.
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "Setting build type to 'Release' as none was specified.")
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Release")
    endif()
endif()

## Dependencies
find_package(MATIO REQUIRED)
find_package(Eigen3 QUIET)

if (Eigen3_FOUND)
    set(MATIOCPP_HAS_EIGEN TRUE)
endif()

# Fetching visit_struct
include(CMakeDependentOption)
find_package(visit_struct QUIET)
option(USE_SYSTEM_visit_struct "Use system visit_struct" ${visit_struct_FOUND})
if(USE_SYSTEM_visit_struct)
     find_package(visit_struct REQUIRED)
else()
    include(FetchContent)
    FetchContent_Declare(visit_struct
      GIT_REPOSITORY https://github.com/ami-iit/visit_struct
      GIT_TAG 47bc6a3aa7588a1f4db39579a0b6812569a76b56)

    FetchContent_GetProperties(visit_struct)
    if(NOT visit_struct_POPULATED)
      message(STATUS "Fetching visit_struct...")
      FetchContent_MakeAvailable(visit_struct)
    endif()
endif()

set(MATIOCPP_SRC src/Variable.cpp
                 src/ConversionUtilities.cpp
                 src/MatvarHandler.cpp
                 src/SharedMatvar.cpp
                 src/WeakMatvar.cpp
                 src/CellArray.cpp
                 src/File.cpp
                 src/Struct.cpp
                 src/StructArray.cpp
                 src/ExogenousConversions.cpp)

set(MATIOCPP_HDR include/matioCpp/Span.h
                 include/matioCpp/VectorIterator.h
                 include/matioCpp/ConversionUtilities.h
                 include/matioCpp/ExogenousConversions.h
                 include/matioCpp/EigenConversions.h
                 include/matioCpp/Variable.h
                 include/matioCpp/ForwardDeclarations.h
                 include/matioCpp/Vector.h
                 include/matioCpp/MultiDimensionalArray.h
                 include/matioCpp/MatvarHandler.h
                 include/matioCpp/SharedMatvar.h
                 include/matioCpp/WeakMatvar.h
                 include/matioCpp/Element.h
                 include/matioCpp/CellArray.h
                 include/matioCpp/File.h
                 include/matioCpp/Struct.h
                 include/matioCpp/StructArray.h
                 include/matioCpp/StructArrayElement.h)

set(MATIOCPP_TPP include/matioCpp/impl/Vector.tpp
                 include/matioCpp/impl/MultiDimensionalArray.tpp
                 include/matioCpp/impl/Element.tpp
                 include/matioCpp/impl/StructArrayElement.tpp
                 include/matioCpp/impl/File.tpp
                 include/matioCpp/impl/EigenConversions.tpp
                 include/matioCpp/impl/ExogenousConversions.tpp
                 include/matioCpp/impl/ExogenousConversionHelpers.tpp)

source_group("Template Implementation Files" FILES ${MATIOCPP_TPP})

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/Autogenerated/matioCpp/Config.h" @ONLY)

list(APPEND MATIOCPP_HDR ${CMAKE_CURRENT_BINARY_DIR}/Autogenerated/matioCpp/Config.h)

# Create the umbrella header
foreach(header ${MATIOCPP_HDR})
  get_filename_component(header_name ${header} NAME)
  set(include_command "#include <matioCpp/${header_name}>")
  set(umbrella_includes_list "${umbrella_includes_list}\n${include_command}")
endforeach()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/matioCpp.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/Autogenerated/matioCpp/matioCpp.h" @ONLY)

list(APPEND MATIOCPP_HDR ${CMAKE_CURRENT_BINARY_DIR}/Autogenerated/matioCpp/matioCpp.h)

# Define the library target
add_library(matioCpp ${MATIOCPP_SRC} ${MATIOCPP_HDR} ${MATIOCPP_TPP})

target_include_directories(matioCpp PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/Autogenerated>"
                                           "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(matioCpp PUBLIC MATIO::MATIO visit_struct::visit_struct)
list(APPEND MATIOCPP_DEPENDENCIES MATIO visit_struct)

if (Eigen3_FOUND)
    target_link_libraries(matioCpp PUBLIC Eigen3::Eigen)
    list(APPEND MATIOCPP_DEPENDENCIES Eigen3)
endif()

target_compile_features(matioCpp PUBLIC cxx_std_14)
if(DISABLE_PERMISSIVE)
    message(STATUS "Adding /permissive- flag.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive-")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /permissive-")
    add_compile_options(/permissive-)
endif()

set_target_properties(matioCpp PROPERTIES
  OUTPUT_NAME matioCpp
  VERSION ${${PROJECT_NAME}_VERSION}
  PUBLIC_HEADER "${MATIOCPP_HDR}"
  PRIVATE_HEADER "${MATIOCPP_TPP}")

add_library(matioCpp::matioCpp ALIAS matioCpp)

# Specify installation targets, typology and destination folders.
install(TARGETS    matioCpp
  EXPORT           matioCppTargets
  COMPONENT        runtime
  LIBRARY          DESTINATION "${CMAKE_INSTALL_LIBDIR}"                   COMPONENT shlib
  ARCHIVE          DESTINATION "${CMAKE_INSTALL_LIBDIR}"                   COMPONENT lib
  RUNTIME          DESTINATION "${CMAKE_INSTALL_BINDIR}"                   COMPONENT bin
  PUBLIC_HEADER    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/matioCpp/"     COMPONENT dev
  PRIVATE_HEADER   DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/matioCpp/impl" COMPONENT dev)

install(FILES "cmake/FindMATIO.cmake" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/matioCpp/cmake")
file(COPY "cmake/FindMATIO.cmake" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/share/matioCpp/cmake")

install(FILES "cmake/Findvisit_struct.cmake" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/matioCpp/cmake")
file(COPY "cmake/Findvisit_struct.cmake" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/share/matioCpp/cmake")

include(InstallBasicPackageFiles)

install_basic_package_files(${PROJECT_NAME}
                            VERSION ${${PROJECT_NAME}_VERSION}
                            EXPORT matioCppTargets
                            COMPATIBILITY AnyNewerVersion
                            VARS_PREFIX ${PROJECT_NAME}
                            NO_CHECK_REQUIRED_COMPONENTS_MACRO
                            DEPENDENCIES ${MATIOCPP_DEPENDENCIES}
                            OVERRIDE_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/share/matioCpp/cmake)
# Add the uninstall target
include(AddUninstallTarget)

if(BUILD_TESTING)
    include(AddUnitTest)
    add_subdirectory(test)
endif()
