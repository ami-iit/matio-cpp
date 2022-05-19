/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

#include <catch2/catch_test_macros.hpp>
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
        matioCpp::Struct var;
    }

    SECTION("Name")
    {
        matioCpp::Struct var("test");

        REQUIRE(var.variableType() == matioCpp::VariableType::Struct);
        REQUIRE(var.valueType() == matioCpp::ValueType::VARIABLE);
    }

    SECTION("Name and elements")
    {
        std::vector<matioCpp::Variable> data;
        data.emplace_back(matioCpp::Vector<double>("vector"));
        data.emplace_back(matioCpp::Element<int>("element"));
        data.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
        data.emplace_back(matioCpp::String("name", "content"));
        data.emplace_back(matioCpp::Struct("otherStruct"));

        matioCpp::Struct var("test", data);

        checkVariable(var, "test", matioCpp::VariableType::Struct,
                      matioCpp::ValueType::VARIABLE, false, {1,1});
    }

    SECTION("Copy constructor")
    {
        std::vector<matioCpp::Variable> data;
        data.emplace_back(matioCpp::Vector<double>("vector"));
        data.emplace_back(matioCpp::Element<int>("element"));
        data.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
        data.emplace_back(matioCpp::String("name", "content"));
        data.emplace_back(matioCpp::Struct("otherStruct"));

        matioCpp::Struct a("test", data);

        matioCpp::Struct b(a);
        REQUIRE(b.variableType() == matioCpp::VariableType::Struct);
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

        matioCpp::Struct a("test", data);

        matioCpp::Struct b(std::move(a));
        REQUIRE(b.variableType() == matioCpp::VariableType::Struct);
        REQUIRE(b.valueType() == matioCpp::ValueType::VARIABLE);
    }

    SECTION("Shared ownership")
    {
        std::vector<size_t> dimensions = {1, 1};
        std::vector<matvar_t*> data;
        data.emplace_back(Mat_VarDuplicate(matioCpp::Vector<double>("vector").toMatio(), 1));
        data.emplace_back(Mat_VarDuplicate(matioCpp::Element<int>("element").toMatio(), 1));
        data.emplace_back(Mat_VarDuplicate(matioCpp::MultiDimensionalArray<double>("array").toMatio(), 1));
        data.emplace_back(Mat_VarDuplicate(matioCpp::String("name", "content").toMatio(), 1));
        data.emplace_back(Mat_VarDuplicate(matioCpp::Struct("otherStruct").toMatio(), 1));
        data.emplace_back(nullptr);

        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_STRUCT, matio_types::MAT_T_STRUCT, 2, dimensions.data(), data.data(), 0);
        REQUIRE(matioVar);

        matioCpp::SharedMatvar sharedMatvar(matioVar);
        matioCpp::Struct sharedVar(sharedMatvar);
        REQUIRE(sharedVar.isValid());
        REQUIRE(sharedVar.toMatio() == matioVar);

        checkVariable(sharedVar, "test", matioCpp::VariableType::Struct, matioCpp::ValueType::VARIABLE, false, dimensions);

        matioCpp::Struct weakVar((matioCpp::WeakMatvar(sharedMatvar)));
        REQUIRE(weakVar.isValid());
        REQUIRE(weakVar.toMatio() == matioVar);

        checkVariable(weakVar, "test", matioCpp::VariableType::Struct, matioCpp::ValueType::VARIABLE, false, dimensions);
    }
}

TEST_CASE("Struct infos")
{
    std::vector<matioCpp::Variable> data;
    data.emplace_back(matioCpp::Vector<double>("vector"));
    data.emplace_back(matioCpp::Element<int>("element"));
    data.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
    data.emplace_back(matioCpp::String("name", "content"));
    data.emplace_back(matioCpp::Struct("otherStruct"));

    matioCpp::Struct test("test", data);

    SECTION("Fields")
    {
        REQUIRE(test.numberOfFields() == 5);

        std::vector<std::string> fields = test.fields();

        REQUIRE(fields.size() == 5);

        REQUIRE(fields[0] == "vector");
        REQUIRE(fields[1] == "element");
        REQUIRE(fields[2] == "array");
        REQUIRE(fields[3] == "name");
        REQUIRE(fields[4] == "otherStruct");

        REQUIRE(test.isFieldExisting("vector"));
        REQUIRE(test.getFieldIndex("vector") == 0);
        REQUIRE(test.isFieldExisting("element"));
        REQUIRE(test.getFieldIndex("element") == 1);
        REQUIRE(test.isFieldExisting("array"));
        REQUIRE(test.getFieldIndex("array") == 2);
        REQUIRE(test.isFieldExisting("name"));
        REQUIRE(test.getFieldIndex("name") == 3);
        REQUIRE(test.isFieldExisting("otherStruct"));
        REQUIRE(test.getFieldIndex("otherStruct") == 4);

        REQUIRE_FALSE(test.isFieldExisting("inexistent"));
    }
}

TEST_CASE("Assignments")
{
    std::vector<matioCpp::Variable> data;
    data.emplace_back(matioCpp::Vector<double>("vector"));
    data.emplace_back(matioCpp::Element<int>("element"));
    data.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
    data.emplace_back(matioCpp::String("name", "content"));
    data.emplace_back(matioCpp::Struct("otherStruct"));

    matioCpp::Struct in("test", data);
    matioCpp::Struct out;

    std::vector<size_t> dimensions = {1, 1};
    std::vector<matvar_t*> pointers;
    pointers.emplace_back(Mat_VarDuplicate(matioCpp::Vector<double>("vector").toMatio(), 1));
    pointers.emplace_back(Mat_VarDuplicate(matioCpp::Element<int>("element").toMatio(), 1));
    pointers.emplace_back(Mat_VarDuplicate(matioCpp::MultiDimensionalArray<double>("array").toMatio(), 1));
    pointers.emplace_back(Mat_VarDuplicate(matioCpp::String("name", "content").toMatio(), 1));
    pointers.emplace_back(Mat_VarDuplicate(matioCpp::Struct("otherStruct").toMatio(), 1));
    pointers.emplace_back(nullptr);

    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_STRUCT, matio_types::MAT_T_STRUCT, 2, dimensions.data(), pointers.data(), 0);
    REQUIRE(matioVar);

    SECTION("From matio")
    {
        matioCpp::Struct var;

        REQUIRE(var.fromMatio(matioVar));

        checkVariable(var, "test", matioCpp::VariableType::Struct, matioCpp::ValueType::VARIABLE, false, dimensions);
    }

    SECTION("Copy assignement")
    {
        matioCpp::Struct another;

        another = in;

        checkSameStruct(another, in);
    }

    SECTION("Move assignement")
    {
        matioCpp::Struct another, yetAnother;

        yetAnother = in;
        another = std::move(yetAnother);
        checkSameStruct(another, in);
    }

    SECTION("From other variable (copy)")
    {
        matioCpp::Variable var(matioVar);
        matioCpp::Struct array;
        REQUIRE(array.fromOther(var));
        checkSameVariable(var, array);
    }

    SECTION("From other variable (move)")
    {
        matioCpp::Variable var(matioVar), var2;
        REQUIRE(var2.fromOther(var));
        matioCpp::Struct array;
        REQUIRE(array.fromOther(std::move(var2)));
        checkSameVariable(var, array);
    }

    SECTION("From vector of Variables")
    {
        matioCpp::Struct imported("test");
        REQUIRE(imported.fromVectorOfVariables(data));
        checkSameStruct(in, imported);
    }

    Mat_VarFree(matioVar);

}

TEST_CASE("Modifications")
{
    std::vector<matioCpp::Variable> data;
    data.emplace_back(matioCpp::Vector<double>("vector", 4));
    data.emplace_back(matioCpp::Element<int>("element"));
    data.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
    data.emplace_back(matioCpp::String("name", "content"));
    data.emplace_back(matioCpp::Struct("otherStruct"));

    matioCpp::Struct in("test", data);
    matioCpp::Struct out;

    out = in;

    REQUIRE(out.setName("t"));
    REQUIRE(out.name() == "t");


    std::vector<double> vectorIn;
    vectorIn.push_back(10);
    vectorIn.push_back(12);
    vectorIn.push_back(14);
    vectorIn.push_back(16);

    in(0).asVector<double>() = vectorIn;

    checkSameVector(in(0).asVector<double>(), vectorIn);

    in["element"].asElement<int>() = 7;
    REQUIRE(in[1].asElement<int>() == 7);

    matioCpp::CellArray cellArray("array");
    REQUIRE(in.setField(cellArray));
    REQUIRE(in[2].variableType() == matioCpp::VariableType::CellArray);

    matioCpp::String anotherString("another", "anotherString");

    matioCpp::Variable previousElement = in(3);
    REQUIRE(previousElement.isValid());

    REQUIRE(in.setField(3, anotherString));
    REQUIRE(in("name").asString()() == "anotherString");
    REQUIRE_FALSE(previousElement.isValid());

    matioCpp::String string("yetAnotherString");
    REQUIRE(in.setField("name", string));
    REQUIRE(in("name").asString()() == "yetAnotherString");


    in.clear();
    REQUIRE(in.numberOfFields() == 0);
}
