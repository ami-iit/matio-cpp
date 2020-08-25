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

template <typename T>
void checkSameVector(const matioCpp::Vector<T>& a, const matioCpp::Vector<T>& b, double precision = 1e-15)
{
    checkVariable(a, b.name(), b.variableType(), b.valueType(), b.isComplex(), b.dimensions());

    for (size_t i = 0; i < a.size(); ++i)
    {
        REQUIRE(std::abs(a(i) - b(i)) < precision);
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
    SECTION("Name")
    {
        matioCpp::Vector<double> var("test");

        REQUIRE(var.variableType() == matioCpp::VariableType::Vector);
        REQUIRE(var.valueType() == matioCpp::ValueType::DOUBLE);
    }

    SECTION("Name and Span")
    {
        std::vector<int64_t> vec(7);
        matioCpp::Vector<int64_t> var("test", matioCpp::make_span(vec));

        REQUIRE(var.variableType() == matioCpp::VariableType::Vector);
        REQUIRE(var.valueType() == matioCpp::ValueType::INT64);
    }

    SECTION("Copy constructor")
    {
        std::vector<char> vec(7);
        matioCpp::Vector<char> a("test", matioCpp::make_span(vec));

        matioCpp::Vector<char> b(a);
        REQUIRE(b.variableType() == matioCpp::VariableType::Vector);
        REQUIRE(b.valueType() == matioCpp::ValueType::UTF8);
    }

    SECTION("Move constructor")
    {
        std::vector<char> vec(7);
        matioCpp::Vector<char> a("test", matioCpp::make_span(vec));

        matioCpp::Vector<char> b(std::move(a));
        REQUIRE(b.variableType() == matioCpp::VariableType::Vector);
        REQUIRE(b.valueType() == matioCpp::ValueType::UTF8);
    }

}

TEST_CASE("Assignments")
{
    std::vector<int> in = {2,4,6,8};
    matioCpp::Vector<int> out("test");
    out = in; //"in" is automatically converted to span!

    std::vector<double> vec(7);
    std::vector<size_t> dimensions = {vec.size(), 1};
    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
    REQUIRE(matioVar);

    SECTION("From matio")
    {

        matioCpp::Vector<double> var;

        REQUIRE(var.fromMatio(matioVar));

        checkVariable(var, "test", matioCpp::VariableType::Vector, matioCpp::ValueType::DOUBLE, false, dimensions);
    }

    SECTION("From existing vector")
    {
        REQUIRE(out.size() == in.size());
        matioCpp::Span<int> correspondingSpan = out.toSpan();
        REQUIRE(in.size() == static_cast<size_t>(correspondingSpan.size()));

        for (size_t i = 0; i < in.size(); ++i)
        {
            REQUIRE(in[i] == correspondingSpan(i));
            REQUIRE(in[i] == out(i));
            REQUIRE(in[i] == out[i]);
        }
    }

    SECTION("Copy assignement")
    {
        matioCpp::Vector<int> another;

        another = out;

        checkSameVector(another, out);
    }

    SECTION("Move assignement")
    {
        matioCpp::Vector<int> another, yetAnother;

        yetAnother = out;
        another = std::move(yetAnother);

        checkSameVector(another, out);
    }

    SECTION("From other variable (copy)")
    {
        matioCpp::Variable var(matioVar);
        matioCpp::Vector<double> vector;
        REQUIRE(vector.fromOther(var));
        checkSameVariable(var, vector);
    }

    SECTION("From other variable (copy)")
    {
        matioCpp::Variable var(matioVar), var2;
        REQUIRE(var2.fromOther(var));
        matioCpp::Vector<double> vector;
        REQUIRE(vector.fromOther(std::move(var2)));
        checkSameVariable(var, vector);
    }

    Mat_VarFree(matioVar);
}

TEST_CASE("Span")
{
    std::vector<int> in = {2,4,6,8};
    matioCpp::Vector<int> out("test");
    out = in;

    checkSameVector(matioCpp::make_span(in), out.toSpan());

    in[0] = 3;
    out.toSpan()[0] = 3;

    checkSameVector(matioCpp::make_span(in), out.toSpan());
}

TEST_CASE("Modifications")
{
    std::vector<int> in = {2,4,6,8};
    matioCpp::Vector<int> out("test");
    out = in;

    REQUIRE(out.setName("test2"));
    REQUIRE(out.name() == "test2");
    checkSameVector(matioCpp::make_span(in), out.toSpan());

    in.push_back(10);
    out.resize(5);
    out(4) = 10;
    checkSameVector(matioCpp::make_span(in), out.toSpan());

    in.pop_back();
    in.pop_back();
    out.resize(3);
    checkSameVector(matioCpp::make_span(in), out.toSpan());
}

TEST_CASE("Data")
{
    std::vector<int> in = {2,4,6,8};
    matioCpp::Vector<int> out("test");
    out = in;

    out.data()[2] = 7;
    in[2] = 7;

    checkSameVector(matioCpp::make_span(in), out.toSpan());
}

