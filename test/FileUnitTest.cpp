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

    for (const std::string& variable : input.variableNames())
    {
        std::cout << variable <<std::endl;
    }
}
