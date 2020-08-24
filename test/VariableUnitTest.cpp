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
#include <vector>
#include <matioCpp/matioCpp.h>

void checkVariable(const matioCpp::Variable& var,
                   const std::string& name,
                   matioCpp::VariableType type,
                   matioCpp::ValueType value,
                   bool complex,
                   const std::vector<size_t>& dimensions)
{
    REQUIRE(var.name() == name);
    REQUIRE(var.variableType() == type);
    REQUIRE(var.valueType() == value);
    REQUIRE(var.isComplex() == complex);
    REQUIRE(var.dimensions().size() == dimensions.size());
    for (size_t i = 0; i < var.dimensions().size(); ++i)
    {
        REQUIRE(var.dimensions()[i] == dimensions[i]);
    }
}

void checkSameVariable(const matioCpp::Variable& a, const matioCpp::Variable& b)
{
    checkVariable(a, b.name(), b.variableType(), b.valueType(), b.isComplex(), b.dimensions());
}

TEST_CASE("Default Constructor")
{
    matioCpp::Variable var;
}

TEST_CASE("From matio")
{
    std::vector<double> vec(7);
    std::vector<size_t> dimensions = {vec.size(), 1};
    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
    REQUIRE(matioVar);

    matioCpp::Variable var(matioVar);

    SECTION("Getters")
    {
        REQUIRE(var.name() == "test");
        REQUIRE(var.variableType() == matioCpp::VariableType::Vector);
        REQUIRE(var.valueType() == matioCpp::ValueType::DOUBLE);
        REQUIRE(var.isComplex() == false);
        REQUIRE(var.dimensions()[0] == vec.size());
        REQUIRE(var.dimensions()[1] == 1);
    }

    Mat_VarFree(matioVar);
}

TEST_CASE("Copy/move constructor/assignment")
{
    std::vector<double> vec(7);
    std::vector<size_t> dimensions = {vec.size(), 1};
    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
    REQUIRE(matioVar);

    SECTION("Copy constructor")
    {
        matioCpp::Variable a;
        REQUIRE(a.fromMatio(matioVar));
        matioCpp::Variable b(a);
        checkSameVariable(a,b);
    }

    SECTION("Copy assignment")
    {
        matioCpp::Variable a;
        REQUIRE(a.fromMatio(matioVar));
        matioCpp::Variable b;
        b = a;
        checkSameVariable(a,b);
    }

    SECTION("Move constructor")
    {
        matioCpp::Variable a;
        REQUIRE(a.fromMatio(matioVar));
        matioCpp::Variable b(std::move(a));
        checkVariable(b, "test", matioCpp::VariableType::Vector,
                      matioCpp::ValueType::DOUBLE, false, {vec.size(), 1});
    }

    SECTION("Move assignment")
    {
        matioCpp::Variable a;
        REQUIRE(a.fromMatio(matioVar));
        matioCpp::Variable b;
        b = std::move(a);
        checkVariable(b, "test", matioCpp::VariableType::Vector,
                      matioCpp::ValueType::DOUBLE, false, {vec.size(), 1});
    }

    Mat_VarFree(matioVar);
}




