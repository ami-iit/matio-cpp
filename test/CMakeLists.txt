# Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

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
