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
    REQUIRE(b.isValid());
    checkVariable(a, b.name(), b.variableType(), b.valueType(), b.isComplex(), b.dimensions());
}

TEST_CASE("Default Constructor")
{
    matioCpp::Variable var;
    REQUIRE_FALSE(var.isValid());
}

TEST_CASE("MatvarHandler")
{
    std::vector<double> vec(7);
    std::vector<size_t> dimensions = {vec.size(), 1};
    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
    REQUIRE(matioVar);

    matioCpp::SharedMatvar sharedMatvar(matioVar);
    matioCpp::Variable sharedVar(sharedMatvar);
    REQUIRE(sharedVar.isValid());
    REQUIRE(sharedVar.toMatio() == matioVar);

    checkVariable(sharedVar, "test", matioCpp::VariableType::Vector,
                  matioCpp::ValueType::DOUBLE, false, dimensions);

    matioCpp::Variable weakVar((matioCpp::WeakMatvar(sharedMatvar)));
    REQUIRE(weakVar.isValid());
    REQUIRE(weakVar.toMatio() == matioVar);

    checkVariable(weakVar, "test", matioCpp::VariableType::Vector,
                  matioCpp::ValueType::DOUBLE, false, dimensions);
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
        REQUIRE(var.isValid());
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
        b.fromOther(a);
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
        b.fromOther(std::move(a));
        checkVariable(b, "test", matioCpp::VariableType::Vector,
                      matioCpp::ValueType::DOUBLE, false, {vec.size(), 1});
    }

    Mat_VarFree(matioVar);
}

TEST_CASE("Complex int")
{
    std::vector<int64_t> real(7);
    std::vector<int64_t> imaginary(real.size());
    std::vector<size_t> dimensions = {real.size(), 1};
    mat_complex_split_t matioComplexSplit;
    matioComplexSplit.Re = real.data();
    matioComplexSplit.Im = imaginary.data();

    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_INT64, dimensions.size(), dimensions.data(), &matioComplexSplit, MAT_F_COMPLEX);
    REQUIRE(matioVar);

    matioCpp::Variable var(matioVar);

    checkVariable(var, "test", matioCpp::VariableType::Vector,
                  matioCpp::ValueType::INT64, true, {real.size(), 1});

    Mat_VarFree(matioVar);
}

TEST_CASE("Conversions")
{
    SECTION("To Vector")
    {
        std::vector<double> vec(7);
        std::vector<size_t> dimensions = {vec.size(), 1};
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
        REQUIRE(matioVar);

        matioCpp::Variable sharedVar((matioCpp::SharedMatvar(matioVar)));
        matioCpp::Vector<double> asVector = sharedVar.asVector<double>();
        checkSameVariable(sharedVar, asVector);
        asVector(0) = 3.14;
        REQUIRE((((double *)(matioVar->data))[0]) == 3.14);

        const matioCpp::Variable& constRef = sharedVar;
        const matioCpp::Vector<double> asConstVector = constRef.asVector<double>();
        REQUIRE(asConstVector(0) == 3.14);
    }

    SECTION("To Multidimensional array")
    {
        std::vector<double> vec(8);
        std::vector<size_t> dimensions = {4,2};
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
        REQUIRE(matioVar);

        matioCpp::Variable sharedVar((matioCpp::SharedMatvar(matioVar)));
        matioCpp::MultiDimensionalArray<double> asArray = sharedVar.asMultiDimensionalArray<double>();
        checkSameVariable(sharedVar, asArray);
        asArray({0,0}) = 3.14;
        REQUIRE((((double *)(matioVar->data))[0]) == 3.14);

        const matioCpp::Variable& constRef = sharedVar;
        const matioCpp::MultiDimensionalArray<double> asConstArray = constRef.asMultiDimensionalArray<double>();
        REQUIRE(asConstArray({0,0}) == 3.14);
    }

    SECTION("To Element")
    {
        double input = 3.14;
        std::vector<size_t> dimensions = {1,1};
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), &input, 0);
        REQUIRE(matioVar);
        matioCpp::Variable sharedVar((matioCpp::SharedMatvar(matioVar)));

        matioCpp::Element<double> asElement = sharedVar.asElement<double>();

        REQUIRE(asElement == 3.14);
    }

    SECTION("To String")
    {
        std::string inputString = "test_string";
        std::vector<size_t> dimensions = {1,inputString.size()};
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_CHAR, matio_types::MAT_T_UTF8, dimensions.size(), dimensions.data(), inputString.data(), 0);
        REQUIRE(matioVar);
        matioCpp::Variable sharedVar((matioCpp::SharedMatvar(matioVar)));

        matioCpp::String asElement = sharedVar.asString();

        REQUIRE(asElement() == "test_string");
    }
}



