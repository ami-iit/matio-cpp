# Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

include_directories(${CMAKE_CURRENT_BINARY_DIR})
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/MatFolderPath.h.in" "${CMAKE_CURRENT_BINARY_DIR}/MatFolderPath.h" @ONLY)

add_unit_test(NAME MatvarHandler
              SOURCES MatvarHandlerUnitTest.cpp
              LINKS matioCpp::matioCpp)

add_unit_test(NAME Span
              SOURCES SpanUnitTest.cpp
              LINKS matioCpp::matioCpp)

add_unit_test(NAME Variable
              SOURCES VariableUnitTest.cpp
              LINKS matioCpp::matioCpp)

add_unit_test(NAME Vector
              SOURCES VectorUnitTest.cpp
              LINKS matioCpp::matioCpp)

add_unit_test(NAME MultiDimensionalArray
              SOURCES MultiDimensionalArrayUnitTest.cpp
              LINKS matioCpp::matioCpp)

add_unit_test(NAME Element
              SOURCES ElementUnitTest.cpp
              LINKS matioCpp::matioCpp)

add_unit_test(NAME CellArray
              SOURCES CellArrayUnitTest.cpp
              LINKS matioCpp::matioCpp)

add_unit_test(NAME File
              SOURCES FileUnitTest.cpp
              LINKS matioCpp::matioCpp)

add_unit_test(NAME Struct
              SOURCES StructUnitTest.cpp
              LINKS matioCpp::matioCpp)

add_unit_test(NAME StructArray
              SOURCES StructArrayUnitTest.cpp
              LINKS matioCpp::matioCpp)

add_unit_test(NAME StructArrayElement
              SOURCES StructArrayElementUnitTest.cpp
              LINKS matioCpp::matioCpp)

