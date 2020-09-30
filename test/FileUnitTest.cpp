/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

#include <catch2/catch.hpp>
#include <matioCpp/matioCpp.h>
#include "MatFolderPath.h"

TEST_CASE("Open file")
{
    matioCpp::File input;

    REQUIRE(input.open(getMatPath("input.mat")));

    auto test = input.read("struct_array");

    matioCpp::String string = input.read("string").asString();
    REQUIRE(string.isValid());
    REQUIRE(string() == "test");

    matioCpp::Element<double> doubleVar = input.read("double").asElement<double>();
    REQUIRE(doubleVar.isValid());
    REQUIRE(doubleVar() == 3.14);

    matioCpp::Element<int> intVar = input.read("int").asElement<int>();
    REQUIRE(intVar.isValid());
    REQUIRE(intVar() == 5);

    for (const std::string& variable : input.variableNames())
    {
        std::cout << variable <<std::endl;
    }
}
