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

void REQUIRE_TRUE(bool value)
{
    REQUIRE(value);
}

void checkSameVariable(const matioCpp::Variable& a, const matioCpp::Variable& b)
{
    checkVariable(a, b.name(), b.variableType(), b.valueType(), b.isComplex(), b.dimensions());
}

void checkSameCellArray(const matioCpp::CellArray& a, const matioCpp::CellArray& b)
{
    checkVariable(a, b.name(), b.variableType(), b.valueType(), b.isComplex(), b.dimensions());

    std::vector<size_t> element(a.dimensions().size());
    element.assign(a.dimensions().size(), 0);

    bool scanned = false;

    while (!scanned)
    {
        //Iterate over the first dimension
        for (element.front() = 0; element.front() < a.dimensions()[0]; ++element.front())
        {
            checkSameVariable(a(element), b(element));
        }

        bool done = false;
        std::ptrdiff_t dimensionToBeIncreased = 1;
        while (!done && (dimensionToBeIncreased < a.dimensions().size())) //Find the smallest dimension to increase without going out of bounds
        {
            if (++element[dimensionToBeIncreased] < a.dimensions()[dimensionToBeIncreased])
            {
                done = true;
            }
            else
            {
                element[dimensionToBeIncreased] = 0;
                dimensionToBeIncreased++;
            }
        }
        scanned = dimensionToBeIncreased == a.dimensions().size();
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
        matioCpp::CellArray var;
    }

    SECTION("Name")
    {
        matioCpp::CellArray var("test");

        REQUIRE(var.variableType() == matioCpp::VariableType::CellArray);
        REQUIRE(var.valueType() == matioCpp::ValueType::VARIABLE);
    }

    SECTION("Name and dimensions")
    {
        matioCpp::CellArray var("test", {1,2,3});

        checkVariable(var, "test", matioCpp::VariableType::CellArray,
                      matioCpp::ValueType::VARIABLE, false, {1,2,3});
    }

    SECTION("Name, dimensions and data")
    {
        std::vector<matioCpp::Variable> data;
        data.emplace_back(matioCpp::Vector<double>("vector"));
        data.emplace_back(matioCpp::Element<int>("element"));
        data.emplace_back(matioCpp::Vector<double>());
        data.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
        data.emplace_back(matioCpp::String("name", "content"));
        data.emplace_back(matioCpp::CellArray("otherCell"));

        matioCpp::CellArray var("test", {1,2,3}, data);

        checkVariable(var, "test", matioCpp::VariableType::CellArray,
                      matioCpp::ValueType::VARIABLE, false, {1,2,3});
    }

    SECTION("Copy constructor")
    {
        std::vector<matioCpp::Variable> data;
        data.emplace_back(matioCpp::Vector<double>("vector"));
        data.emplace_back(matioCpp::Element<int>("element"));
        data.emplace_back(matioCpp::Vector<double>());
        data.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
        data.emplace_back(matioCpp::String("name", "content"));
        data.emplace_back(matioCpp::CellArray("otherCell"));

        matioCpp::CellArray a("test", {1,2,3}, data);

        matioCpp::CellArray b(a);
        REQUIRE(b.variableType() == matioCpp::VariableType::CellArray);
        REQUIRE(b.valueType() == matioCpp::ValueType::VARIABLE);
    }

    SECTION("Move constructor")
    {
        std::vector<matioCpp::Variable> data;
        data.emplace_back(matioCpp::Vector<double>("vector"));
        data.emplace_back(matioCpp::Element<int>("element"));
        data.emplace_back(matioCpp::Vector<double>());
        data.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
        data.emplace_back(matioCpp::String("name", "content"));
        data.emplace_back(matioCpp::CellArray("otherCell"));

        matioCpp::CellArray a("test", {1,2,3}, data);

        matioCpp::CellArray b(std::move(a));
        REQUIRE(b.variableType() == matioCpp::VariableType::CellArray);
        REQUIRE(b.valueType() == matioCpp::ValueType::VARIABLE);
    }

    SECTION("Shared ownership")
    {
        std::vector<matvar_t*> data;
        std::vector<size_t> dimensions = {1,2,3};
        data.emplace_back(Mat_VarDuplicate(matioCpp::Vector<double>("vector").toMatio(), 1));
        data.emplace_back(Mat_VarDuplicate(matioCpp::Element<int>("element").toMatio(), 1));
        data.emplace_back(nullptr);
        data.emplace_back(Mat_VarDuplicate(matioCpp::MultiDimensionalArray<double>("array").toMatio(), 1));
        data.emplace_back(Mat_VarDuplicate(matioCpp::String("name", "content").toMatio(), 1));
        data.emplace_back(Mat_VarDuplicate(matioCpp::CellArray("otherCell").toMatio(), 1));

        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_CELL, matio_types::MAT_T_CELL, dimensions.size(), dimensions.data(), data.data(), 0);
        REQUIRE(matioVar);

        matioCpp::SharedMatvar sharedMatvar(matioVar);
        matioCpp::CellArray sharedVar(sharedMatvar);
        REQUIRE(sharedVar.isValid());
        REQUIRE(sharedVar.toMatio() == matioVar);

        checkVariable(sharedVar, "test", matioCpp::VariableType::CellArray, matioCpp::ValueType::VARIABLE, false, dimensions);

        matioCpp::CellArray weakVar((matioCpp::WeakMatvar(sharedMatvar)));
        REQUIRE(weakVar.isValid());
        REQUIRE(weakVar.toMatio() == matioVar);

        checkVariable(weakVar, "test", matioCpp::VariableType::CellArray, matioCpp::ValueType::VARIABLE, false, dimensions);
    }

}

TEST_CASE("Indeces from raw index")
{
    matioCpp::CellArray var("test", {3,4,5});

    REQUIRE(var.numberOfElements() == 60);

    for (size_t i = 0; i < var.numberOfElements(); ++i)
    {
        std::vector<size_t> el;
        REQUIRE(var.indicesFromRawIndex(i, el));
        REQUIRE(var.rawIndexFromIndices(el) == i);
    }

}

TEST_CASE("Assignments")
{
    std::vector<matioCpp::Variable> data;
    data.emplace_back(matioCpp::Vector<double>("vector"));
    data.emplace_back(matioCpp::Element<int>("element"));
    data.emplace_back(matioCpp::Vector<double>());
    data.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
    data.emplace_back(matioCpp::String("name", "content"));
    data.emplace_back(matioCpp::CellArray("otherCell"));

    matioCpp::CellArray in("test", {1,2,3}, data);
    matioCpp::CellArray out;

    std::vector<matvar_t*> pointers;
    std::vector<size_t> dimensions = {1,2,3};
    pointers.emplace_back(Mat_VarDuplicate(matioCpp::Vector<double>("vector").toMatio(), 1));
    pointers.emplace_back(Mat_VarDuplicate(matioCpp::Element<int>("element").toMatio(), 1));
    pointers.emplace_back(nullptr);
    pointers.emplace_back(Mat_VarDuplicate(matioCpp::MultiDimensionalArray<double>("array").toMatio(), 1));
    pointers.emplace_back(Mat_VarDuplicate(matioCpp::String("name", "content").toMatio(), 1));
    pointers.emplace_back(Mat_VarDuplicate(matioCpp::CellArray("otherCell").toMatio(), 1));
    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_CELL, matio_types::MAT_T_CELL, dimensions.size(), dimensions.data(), pointers.data(), 0);
    REQUIRE(matioVar);

    SECTION("From matio")
    {

        matioCpp::CellArray var;

        REQUIRE(var.fromMatio(matioVar));

        checkVariable(var, "test", matioCpp::VariableType::CellArray, matioCpp::ValueType::VARIABLE, false, dimensions);
    }

    SECTION("Copy assignement")
    {
        matioCpp::CellArray another;

        another = in;

        checkSameCellArray(another, in);
    }

    SECTION("Move assignement")
    {
        matioCpp::CellArray another, yetAnother;

        yetAnother = in;
        another = std::move(yetAnother);
        checkSameCellArray(another, in);
    }

    SECTION("From other variable (copy)")
    {
        matioCpp::Variable var(matioVar);
        matioCpp::CellArray array;
        REQUIRE(array.fromOther(var));
        checkSameVariable(var, array);
    }

    SECTION("From other variable (move)")
    {
        matioCpp::Variable var(matioVar), var2;
        REQUIRE(var2.fromOther(var));
        matioCpp::CellArray array;
        REQUIRE(array.fromOther(std::move(var2)));
        checkSameVariable(var, array);
    }

    SECTION("From vector of Variables")
    {
        matioCpp::CellArray imported("test");
        REQUIRE(imported.fromVectorOfVariables({1,2,3}, data));
        checkSameCellArray(in, imported);
    }

    Mat_VarFree(matioVar);
}

TEST_CASE("Modifications")
{
    std::vector<matioCpp::Variable> data;
    data.emplace_back(matioCpp::Vector<double>("vector", 4));
    data.emplace_back(matioCpp::Element<int>("element"));
    data.emplace_back(matioCpp::Vector<double>());
    data.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
    data.emplace_back(matioCpp::String("name", "content"));
    data.emplace_back(matioCpp::CellArray("otherCell"));

    matioCpp::CellArray in("test", {1,2,3}, data);
    matioCpp::CellArray out;

    REQUIRE(out.setName("test2"));
    REQUIRE(out.name() == "test2");

    out.resize({2,2,2,2});

    REQUIRE(out.dimensions().size() == 4);
    REQUIRE(out.dimensions()[0] == 2);
    REQUIRE(out.dimensions()[1] == 2);
    REQUIRE(out.dimensions()[2] == 2);
    REQUIRE(out.dimensions()[3] == 2);
    REQUIRE(out.numberOfElements() == 16);


    std::vector<double> vectorIn;
    vectorIn.push_back(10);
    vectorIn.push_back(12);
    vectorIn.push_back(14);
    vectorIn.push_back(16);

    in({0,0,0}).asVector<double>() = vectorIn;

    checkSameVector(in({0,0,0}).asVector<double>(), vectorIn);

    in[{0,1,0}].asElement<int>() = 7;
    REQUIRE(in[1].asElement<int>() == 7);

    matioCpp::String anotherString("another", "anotherString");

    REQUIRE(in.setElement(4, anotherString));
    REQUIRE(in(4).asString()() == "anotherString");

}

