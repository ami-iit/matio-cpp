/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

#include <catch2/catch_test_macros.hpp>
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

TEST_CASE("Constructors")
{
    SECTION("Default")
    {
        matioCpp::Element<int8_t> i8;
        REQUIRE(i8.isValid());
        REQUIRE(i8.valueType() == matioCpp::ValueType::INT8);

        matioCpp::Element<uint8_t> ui8;
        REQUIRE(ui8.isValid());
        REQUIRE(ui8.valueType() == matioCpp::ValueType::UINT8);

        matioCpp::Element<int16_t> i16;
        REQUIRE(i16.isValid());
        REQUIRE(i16.valueType() == matioCpp::ValueType::INT16);

        matioCpp::Element<uint16_t> ui16;
        REQUIRE(ui16.isValid());
        REQUIRE(ui16.valueType() == matioCpp::ValueType::UINT16);

        matioCpp::Element<int32_t> i32;
        REQUIRE(i32.isValid());
        REQUIRE(i32.valueType() == matioCpp::ValueType::INT32);

        matioCpp::Element<int> i;
        REQUIRE(i.isValid());
        REQUIRE(i.valueType() == matioCpp::ValueType::INT32);

        matioCpp::Element<uint32_t> ui32;
        REQUIRE(ui32.isValid());
        REQUIRE(ui32.valueType() == matioCpp::ValueType::UINT32);

        matioCpp::Element<float> f;
        REQUIRE(f.isValid());
        REQUIRE(f.valueType() == matioCpp::ValueType::SINGLE);

        matioCpp::Element<double> d;
        REQUIRE(d.isValid());
        REQUIRE(d.valueType() == matioCpp::ValueType::DOUBLE);

        matioCpp::Element<int64_t> i64;
        REQUIRE(i64.isValid());
        REQUIRE(i64.valueType() == matioCpp::ValueType::INT64);

        matioCpp::Element<size_t> ui64;
        REQUIRE(ui64.isValid());
        REQUIRE(ui64.valueType() == matioCpp::ValueType::UINT64);

        matioCpp::Element<char> c;
        REQUIRE(c.isValid());
        REQUIRE(c.valueType() == matioCpp::ValueType::UTF8);

        matioCpp::Element<char16_t> c16;
        REQUIRE(c16.isValid());
        REQUIRE(c16.valueType() == matioCpp::ValueType::UTF16);

        matioCpp::Element<char32_t> c32;
        REQUIRE(c32.isValid());
        REQUIRE(c32.valueType() == matioCpp::ValueType::UTF32);

        matioCpp::Element<matioCpp::Logical> logical;
        REQUIRE(logical.isValid());
        REQUIRE(logical.valueType() == matioCpp::ValueType::LOGICAL);
    }

    SECTION("Name")
    {
        matioCpp::Element<double> d("hello_element");
        REQUIRE(d.isValid());
        REQUIRE(d.valueType() == matioCpp::ValueType::DOUBLE);
        REQUIRE(d.name() == "hello_element");
    }

    SECTION("Name and value")
    {
        matioCpp::Element<int> i("hello_element", 7);
        REQUIRE(i.isValid());
        REQUIRE(i.valueType() == matioCpp::ValueType::INT32);
        REQUIRE(i.name() == "hello_element");
        REQUIRE((((int *)(i.toMatio()->data))[0]) == 7);
    }

    SECTION("Copy constructor")
    {
        matioCpp::Element<int> i("hello_element", 7);
        matioCpp::Element<int> iCopy(i);
        checkSameVariable(i, iCopy);
    }

    SECTION("Move constructor")
    {
        matioCpp::Element<int> i("hello_element", 7);
        matioCpp::Element<int> iCopy(std::move(i));
        REQUIRE(iCopy.isValid());
        REQUIRE(iCopy.valueType() == matioCpp::ValueType::INT32);
        REQUIRE(iCopy.name() == "hello_element");
        REQUIRE((((int *)(iCopy.toMatio()->data))[0]) == 7);
    }

    SECTION("Shared ownership")
    {
        double input = 7;
        std::vector<size_t> dimensions = {1,1};
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, static_cast<int>(dimensions.size()), dimensions.data(), &input, 0);
        REQUIRE(matioVar);

        matioCpp::SharedMatvar sharedMatvar(matioVar);
        matioCpp::Element<double> sharedVar(sharedMatvar);
        REQUIRE(sharedVar.isValid());
        REQUIRE(sharedVar.toMatio() == matioVar);

        checkVariable(sharedVar, "test", matioCpp::VariableType::Element, matioCpp::ValueType::DOUBLE, false, dimensions);

        matioCpp::Element<double> weakVar((matioCpp::WeakMatvar(sharedMatvar)));
        REQUIRE(weakVar.isValid());
        REQUIRE(weakVar.toMatio() == matioVar);

        checkVariable(weakVar, "test", matioCpp::VariableType::Element, matioCpp::ValueType::DOUBLE, false, dimensions);
    }
}

TEST_CASE("Assignements")
{
    SECTION("Copy")
    {
        matioCpp::Element<int> i("hello_element", 7);
        matioCpp::Element<int> iCopy;

        iCopy = i;
        checkSameVariable(i, iCopy);
    }

    SECTION("Move")
    {
        matioCpp::Element<int> i("hello_element", 7);
        matioCpp::Element<int> iCopy;

        iCopy = std::move(i);
        REQUIRE(iCopy.isValid());
        REQUIRE(iCopy.valueType() == matioCpp::ValueType::INT32);
        REQUIRE(iCopy.name() == "hello_element");
        REQUIRE((((int *)(iCopy.toMatio()->data))[0]) == 7);
    }

    SECTION("From element")
    {
        matioCpp::Element<int> i("hello_element");

        i = 8;
        REQUIRE((((int *)(i.toMatio()->data))[0]) == 8);

    }
}

TEST_CASE("Span")
{
    matioCpp::Element<int> i("hello_element", 7);

    auto span = i.toSpan();
    REQUIRE(span.size() == 1);
    REQUIRE(span[0] == 7);

    const matioCpp::Span<const int> constSpan = i.toSpan();
    REQUIRE(constSpan.size() == 1);
    REQUIRE(constSpan[0] == 7);
}

TEST_CASE("Set name")
{
    matioCpp::Element<int> i("hello_element", 7);

    i.setName("new_hello_element");
    REQUIRE(i.isValid());
    REQUIRE(i.valueType() == matioCpp::ValueType::INT32);
    REQUIRE(i.name() == "new_hello_element");
    REQUIRE((((int *)(i.toMatio()->data))[0]) == 7);
}

TEST_CASE("Operator ()")
{
    matioCpp::Element<int> i("hello_element", 7);

    REQUIRE(i() == 7);
    i() = 8;
    REQUIRE(i() == 8);
}

TEST_CASE("Casting")
{
    matioCpp::Element<int> i("hello_element", 7);
    int output;

    output = i;
    REQUIRE(output == 7);
}
