/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
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

template <typename T>
void checkSameMultiDimensionalArray(const matioCpp::MultiDimensionalArray<T>& a, const matioCpp::MultiDimensionalArray<T>& b, double precision = 1e-15)
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
            REQUIRE(std::abs(a(element) - b(element)) < precision);
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

template <typename T>
void checkSameVector(const T& a, const T& b, double precision = 1e-15)
{
    REQUIRE(a.size() == b.size());

    for (size_t i = 0; i < static_cast<size_t>(a.size()); ++i)
    {
        REQUIRE(std::abs(a(i) - b(i)) < precision);
    }
}

TEST_CASE("Constructors")
{
    SECTION("Default")
    {
        matioCpp::MultiDimensionalArray<matioCpp::Logical> test;
    }

    SECTION("Name")
    {
        matioCpp::MultiDimensionalArray<double> var("test");

        REQUIRE(var.variableType() == matioCpp::VariableType::MultiDimensionalArray);
        REQUIRE(var.valueType() == matioCpp::ValueType::DOUBLE);
    }

    SECTION("Name and dimensions")
    {
        matioCpp::MultiDimensionalArray<int64_t> var("test", {1,2,3});

        checkVariable(var, "test", matioCpp::VariableType::MultiDimensionalArray,
                      matioCpp::ValueType::INT64, false, {1,2,3});
    }

    SECTION("Name, dimensions and data")
    {
        std::vector<int64_t> data(6);
        matioCpp::MultiDimensionalArray<int64_t> var("test", {1,2,3}, data.data());

        checkVariable(var, "test", matioCpp::VariableType::MultiDimensionalArray,
                      matioCpp::ValueType::INT64, false, {1,2,3});
    }

    SECTION("Copy constructor")
    {
        std::vector<char> data(6);
        matioCpp::MultiDimensionalArray<char> a("test", {1,2,3}, data.data());

        matioCpp::MultiDimensionalArray<char> b(a);
        REQUIRE(b.variableType() == matioCpp::VariableType::MultiDimensionalArray);
        REQUIRE(b.valueType() == matioCpp::ValueType::UTF8);
    }

    SECTION("Move constructor")
    {
        std::vector<char> data(6);
        matioCpp::MultiDimensionalArray<char> a("test", {1,2,3}, data.data());

        matioCpp::MultiDimensionalArray<char> b(std::move(a));
        REQUIRE(b.variableType() == matioCpp::VariableType::MultiDimensionalArray);
        REQUIRE(b.valueType() == matioCpp::ValueType::UTF8);
    }

    SECTION("Shared ownership")
    {
        std::vector<double> vec(8);
        std::vector<size_t> dimensions = {4,2};
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, static_cast<int>(dimensions.size()), dimensions.data(), vec.data(), 0);
        REQUIRE(matioVar);

        matioCpp::SharedMatvar sharedMatvar(matioVar);
        matioCpp::MultiDimensionalArray<double> sharedVar(sharedMatvar);
        REQUIRE(sharedVar.isValid());
        REQUIRE(sharedVar.toMatio() == matioVar);

        checkVariable(sharedVar, "test", matioCpp::VariableType::MultiDimensionalArray, matioCpp::ValueType::DOUBLE, false, dimensions);

        matioCpp::MultiDimensionalArray<double> weakVar((matioCpp::WeakMatvar(sharedMatvar)));
        REQUIRE(weakVar.isValid());
        REQUIRE(weakVar.toMatio() == matioVar);

        checkVariable(weakVar, "test", matioCpp::VariableType::MultiDimensionalArray, matioCpp::ValueType::DOUBLE, false, dimensions);
    }

}

TEST_CASE("Indeces from raw index")
{
    matioCpp::MultiDimensionalArray<double> var("test", {3,4,5});

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
    std::vector<int> dataVec = {2,4,6,8};
    matioCpp::MultiDimensionalArray<int> in("test", {2,2}, dataVec.data());
    matioCpp::MultiDimensionalArray<int> out;

    std::vector<double> vec(8);
    std::vector<size_t> dimensions = {4,2};
    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, static_cast<int>(dimensions.size()), dimensions.data(), vec.data(), 0);
    REQUIRE(matioVar);

    SECTION("From matio")
    {

        matioCpp::MultiDimensionalArray<double> var;

        REQUIRE(var.fromMatio(matioVar));

        checkVariable(var, "test", matioCpp::VariableType::MultiDimensionalArray, matioCpp::ValueType::DOUBLE, false, dimensions);
    }

    SECTION("From existing vector")
    {
        REQUIRE(out.fromVectorizedArray({2,2}, dataVec.data()));
        checkSameDimensions(in.dimensions(), out.dimensions());
        matioCpp::Span<int> correspondingSpan = out.toSpan();
        REQUIRE(dataVec.size() == static_cast<size_t>(correspondingSpan.size()));

        for (size_t i = 0; i < dataVec.size(); ++i)
        {
            REQUIRE(dataVec[i] == correspondingSpan(i));
        }
        REQUIRE(out.numberOfElements() == dataVec.size());
    }

    SECTION("Copy assignement")
    {
        matioCpp::MultiDimensionalArray<int> another;

        another = in;

        checkSameMultiDimensionalArray(another, in);
    }

    SECTION("Move assignement")
    {
        matioCpp::MultiDimensionalArray<int> another, yetAnother;

        yetAnother = in;
        another = std::move(yetAnother);

        checkSameMultiDimensionalArray(another, in);
    }

    SECTION("From other variable (copy)")
    {
        matioCpp::Variable var(matioVar);
        matioCpp::MultiDimensionalArray<double> array;
        REQUIRE(array.fromOther(var));
        checkSameVariable(var, array);
        matioCpp::Vector<double> vec("test", 5);
        REQUIRE(array.fromOther(vec));
        matioCpp::Element<double> el("el", 7);
        REQUIRE(array.fromOther(el));
    }

    SECTION("From other variable (move)")
    {
        matioCpp::Variable var(matioVar), var2;
        REQUIRE(var2.fromOther(var));
        matioCpp::MultiDimensionalArray<double> array;
        REQUIRE(array.fromOther(std::move(var2)));
        checkSameVariable(var, array);
    }

    Mat_VarFree(matioVar);
}

TEST_CASE("Span")
{
    std::vector<int> in = {2,4,6,8};
    matioCpp::MultiDimensionalArray<int> out;
    REQUIRE(out.fromVectorizedArray({2,2}, in.data()));

    checkSameVector(matioCpp::make_span(in), out.toSpan());

    REQUIRE(out({0,0}) == 2);
    REQUIRE(out({1,0}) == 4);
    REQUIRE(out({0,1}) == 6);
    REQUIRE(out({1,1}) == 8);

    in[0] = 3;
    out.toSpan()[0] = 3;

    checkSameVector(matioCpp::make_span(in), out.toSpan());
}

TEST_CASE("Modifications")
{
    std::vector<int> in = {2,4,6,8};
    matioCpp::MultiDimensionalArray<int> out("test");
    REQUIRE(out.fromVectorizedArray({2,2}, in.data()));

    REQUIRE(out.setName("test2"));
    REQUIRE(out.name() == "test2");
    checkSameVector(matioCpp::make_span(in), out.toSpan());

    in.push_back(10);
    in.push_back(12);
    in.push_back(14);
    in.push_back(16);

    out.resize({2,2,2});
    REQUIRE(out.numberOfElements() == 8);
    out({0,0,0}) = 2;
    out({1,0,0}) = 4;
    out({0,1,0}) = 6;
    out({1,1,0}) = 8;
    out({0,0,1}) = 10;
    out({1,0,1}) = 12;
    out({0,1,1}) = 14;
    out({1,1,1}) = 16;

    REQUIRE(out({0,0,0}) == 2);
    REQUIRE(out({1,0,0}) == 4);
    REQUIRE(out({0,1,0}) == 6);
    REQUIRE(out({1,1,0}) == 8);
    REQUIRE(out({0,0,1}) == 10);
    REQUIRE(out({1,0,1}) == 12);
    REQUIRE(out({0,1,1}) == 14);
    REQUIRE(out({1,1,1}) == 16);

    REQUIRE(out(0) == 2);
    REQUIRE(out(1) == 4);
    REQUIRE(out(2) == 6);
    REQUIRE(out(3) == 8);
    REQUIRE(out(4) == 10);
    REQUIRE(out[5] == 12);
    REQUIRE(out[6] == 14);
    REQUIRE(out[7] == 16);

    checkSameVector(matioCpp::make_span(in), out.toSpan());

    out.resize({1,5}); //Resizing as it was a vector
    REQUIRE(out.isValid());
    REQUIRE(out.dimensions().size() == 2);
    REQUIRE(out.dimensions()[0] == 1);
    REQUIRE(out.dimensions()[1] == 5);
    REQUIRE(out.asVector<int>().isValid());

    out.clear();
    REQUIRE(out.numberOfElements() == 0);
}

TEST_CASE("Data")
{
    std::vector<int> in = {2,4,6,8};
    matioCpp::MultiDimensionalArray<int> out("test");
    REQUIRE(out.fromVectorizedArray({2,2}, in.data()));

    out.data()[2] = 7;
    in[2] = 7;

    checkSameVector(matioCpp::make_span(in), out.toSpan());
}

