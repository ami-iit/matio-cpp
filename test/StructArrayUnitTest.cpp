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

void checkSameDimensions(matioCpp::Span<const size_t> a, matioCpp::Span<const size_t> b)
{
    REQUIRE(a.size() == b.size());

    for (size_t i = 0; i < static_cast<size_t>(a.size()); ++i)
    {
        REQUIRE(a[i] == b[i]);
    }
}

void checkVariable(const matioCpp::Variable& var,
                   const std::string& name,
                   matioCpp::VariableType type,
                   matioCpp::ValueType value,
                   bool complex,
                   matioCpp::Span<const size_t> dimensions)
{
    REQUIRE(var.name() == name);
    REQUIRE(var.variableType() == type);
    REQUIRE(var.valueType() == value);
    REQUIRE(var.isComplex() == complex);
    REQUIRE(var.dimensions().size() == dimensions.size());
    checkSameDimensions(dimensions, var.dimensions());
}

void checkVariable(const matioCpp::Variable& var,
                   const std::string& name,
                   matioCpp::VariableType type,
                   matioCpp::ValueType value,
                   bool complex,
                   const std::vector<size_t>& dimensions)
{
    checkVariable(var, name, type, value, complex, matioCpp::make_span(dimensions));
}

void checkSameVariable(const matioCpp::Variable& a, const matioCpp::Variable& b)
{
    checkVariable(a, b.name(), b.variableType(), b.valueType(), b.isComplex(), b.dimensions());
}

void checkSameStruct(const matioCpp::Struct& a, const matioCpp::Struct& b)
{
    checkVariable(a, b.name(), b.variableType(), b.valueType(), b.isComplex(), b.dimensions());

    REQUIRE(a.numberOfFields() == b.numberOfFields());

    for (size_t i = 0; i < a.numberOfFields(); ++i)
    {
        checkSameVariable(a, b);
    }
}

template <typename T1, typename T2>
void checkSameVector(const T1& a, const T2& b, double precision = 1e-15)
{
    REQUIRE(a.size() == b.size());

    for (size_t i = 0; i < static_cast<size_t>(a.size()); ++i)
    {
        REQUIRE(std::abs(a[i] - b[i]) < precision);
    }
}

TEST_CASE("Constructors")
{
    SECTION("Default")
    {
        matioCpp::StructArray var;
    }

    SECTION("Name")
    {
        matioCpp::StructArray var("test");

        REQUIRE(var.variableType() == matioCpp::VariableType::StructArray);
        REQUIRE(var.valueType() == matioCpp::ValueType::VARIABLE);
    }

    SECTION("Name and dimensions")
    {
        matioCpp::StructArray var("test", {1,2,3});

        checkVariable(var, "test", matioCpp::VariableType::StructArray,
                      matioCpp::ValueType::VARIABLE, false, {1,2,3});
    }

    SECTION("Name, dimensions and data")
    {
        std::vector<matioCpp::Variable> data;
        data.emplace_back(matioCpp::Vector<double>("vector"));
        data.emplace_back(matioCpp::Element<int>("element"));
        data.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
        data.emplace_back(matioCpp::String("name", "content"));
        data.emplace_back(matioCpp::Struct("otherStruct"));

        matioCpp::Struct structVar("test", data);

        std::vector<matioCpp::Struct> structVector(6, structVar);

        matioCpp::StructArray var("test", {1,2,3}, structVector);

        checkVariable(var, "test", matioCpp::VariableType::StructArray,
                      matioCpp::ValueType::VARIABLE, false, {1,2,3});
    }

    SECTION("Copy constructor")
    {
        std::vector<matioCpp::Variable> data;
        data.emplace_back(matioCpp::Vector<double>("vector"));
        data.emplace_back(matioCpp::Element<int>("element"));
        data.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
        data.emplace_back(matioCpp::String("name", "content"));
        data.emplace_back(matioCpp::Struct("otherStruct"));

        matioCpp::Struct structVar("test", data);

        std::vector<matioCpp::Struct> structVector(6, structVar);

        matioCpp::StructArray a("test", {1,2,3}, structVector);

        matioCpp::StructArray b(a);
        REQUIRE(b.variableType() == matioCpp::VariableType::StructArray);
        REQUIRE(b.valueType() == matioCpp::ValueType::VARIABLE);
    }

    SECTION("Move constructor")
    {
        std::vector<matioCpp::Variable> data;
        data.emplace_back(matioCpp::Vector<double>("vector"));
        data.emplace_back(matioCpp::Element<int>("element"));
        data.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
        data.emplace_back(matioCpp::String("name", "content"));
        data.emplace_back(matioCpp::Struct("otherStruct"));

        matioCpp::Struct structVar("test", data);

        std::vector<matioCpp::Struct> structVector(6, structVar);

        matioCpp::StructArray a("test", {1,2,3}, structVector);

        matioCpp::StructArray b(std::move(a));
        REQUIRE(b.variableType() == matioCpp::VariableType::StructArray);
        REQUIRE(b.valueType() == matioCpp::ValueType::VARIABLE);
    }

    SECTION("Shared ownership")
    {
        std::vector<size_t> dimensions = {2,2};
        std::vector<matvar_t*> pointers;
        pointers.emplace_back(matioCpp::MatvarHandler::GetMatvarDuplicate(matioCpp::Vector<double>("vector").toMatio()));
        pointers.emplace_back(matioCpp::MatvarHandler::GetMatvarDuplicate(matioCpp::Element<int>("element").toMatio()));
        pointers.emplace_back(matioCpp::MatvarHandler::GetMatvarDuplicate(matioCpp::MultiDimensionalArray<double>("array").toMatio()));
        pointers.emplace_back(matioCpp::MatvarHandler::GetMatvarDuplicate(matioCpp::String("name", "content").toMatio()));
        pointers.emplace_back(matioCpp::MatvarHandler::GetMatvarDuplicate(matioCpp::Struct("otherStruct").toMatio()));
        std::vector<matvar_t*> pointersArray;
        pointersArray.insert(pointersArray.end(), pointers.begin(), pointers.end());
        for (size_t k = 1; k < 4; ++k)
        {
            for (size_t i = 0; i < pointers.size(); ++i)
            {
                pointersArray.emplace_back(matioCpp::MatvarHandler::GetMatvarDuplicate(pointers[i]));
            }
        }
        pointersArray.emplace_back(nullptr);

        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_STRUCT, matio_types::MAT_T_STRUCT, dimensions.size(), dimensions.data(), pointersArray.data(), 0);
        REQUIRE(matioVar);

        matioCpp::SharedMatvar sharedMatvar(matioVar);
        matioCpp::StructArray sharedVar(sharedMatvar);
        REQUIRE(sharedVar.isValid());
        REQUIRE(sharedVar.toMatio() == matioVar);

        checkVariable(sharedVar, "test", matioCpp::VariableType::StructArray, matioCpp::ValueType::VARIABLE, false, dimensions);

        matioCpp::StructArray weakVar((matioCpp::WeakMatvar(sharedMatvar)));
        REQUIRE(weakVar.isValid());
        REQUIRE(weakVar.toMatio() == matioVar);

        checkVariable(weakVar, "test", matioCpp::VariableType::StructArray, matioCpp::ValueType::VARIABLE, false, dimensions);
    }

}


