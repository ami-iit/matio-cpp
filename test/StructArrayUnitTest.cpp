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

void checkSameStructArray(const matioCpp::StructArray& a, const matioCpp::StructArray& b)
{
    checkSameVariable(a, b);

    REQUIRE(a.numberOfFields() == b.numberOfFields());
    checkSameDimensions(a.dimensions(), b.dimensions());

    for (size_t i = 0; i < a.numberOfElements(); ++i)
    {
        checkSameVariable(a[i].asStruct(), b[i].asStruct());
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

    SECTION("Name, dimensions and fields")
    {
        matioCpp::StructArray var("test", {1,2,3}, {"a", "b", "c"});

        checkVariable(var, "test", matioCpp::VariableType::StructArray,
                      matioCpp::ValueType::VARIABLE, false, {1,2,3});
        REQUIRE(var.numberOfFields() == 3);
        std::vector<std::string> fields = var.fields();
        REQUIRE(fields[0] == "a");
        REQUIRE(fields[1] == "b");
        REQUIRE(fields[2] == "c");
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

        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_STRUCT, matio_types::MAT_T_STRUCT, static_cast<int>(dimensions.size()), dimensions.data(), pointersArray.data(), 0);
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

TEST_CASE("Indeces from raw index")
{
    matioCpp::StructArray var("test", {3,4,5});

    REQUIRE(var.numberOfElements() == 60);

    for (size_t i = 0; i < var.numberOfElements(); ++i)
    {
        std::vector<size_t> el;
        REQUIRE(var.indicesFromRawIndex(i, el));
        REQUIRE(var.rawIndexFromIndices(el) == i);
    }

}

TEST_CASE("Assignments and modifications")
{
    std::vector<matioCpp::Variable> data;
    data.emplace_back(matioCpp::Vector<double>("vector", 4));
    data.emplace_back(matioCpp::Element<int>("element", 1));
    data.emplace_back(matioCpp::MultiDimensionalArray<double>("array", {2,2}));
    data.emplace_back(matioCpp::String("name", "content"));

    matioCpp::Struct structVar("test", data);

    std::vector<matioCpp::Struct> structVector(6, structVar);

    matioCpp::StructArray in("test", {1,2,3}, structVector);
    matioCpp::StructArray out("test");

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

    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_STRUCT, matio_types::MAT_T_STRUCT, static_cast<int>(dimensions.size()), dimensions.data(), pointersArray.data(), 0);
    REQUIRE(matioVar);

    SECTION("Assignements")
    {
        SECTION("From matio")
        {

            matioCpp::StructArray var;

            REQUIRE(var.fromMatio(matioVar));

            checkVariable(var, "test", matioCpp::VariableType::StructArray, matioCpp::ValueType::VARIABLE, false, dimensions);
        }

        SECTION("From vector of structs")
        {
            REQUIRE(out.fromVectorOfStructs({1,2,3}, structVector));
            checkSameDimensions(in.dimensions(), out.dimensions());
            REQUIRE(out.numberOfElements() == structVector.size());
            checkSameStructArray(in, out);

        }

        SECTION("Copy assignement")
        {
            matioCpp::StructArray another;

            another = in;

            checkSameStructArray(another, in);
        }

        SECTION("Move assignement")
        {
            matioCpp::StructArray another, yetAnother;

            yetAnother = in;
            another = std::move(yetAnother);

            checkSameStructArray(another, in);
        }

        SECTION("From other variable (copy)")
        {
            matioCpp::Variable var(matioVar);
            matioCpp::StructArray array;
            REQUIRE(array.fromOther(var));
            checkSameVariable(var, array);
        }

        SECTION("From other variable (move)")
        {
            matioCpp::Variable var(matioVar), var2;
            REQUIRE(var2.fromOther(var));
            matioCpp::StructArray array;
            REQUIRE(array.fromOther(std::move(var2)));
            checkSameVariable(var, array);
        }
    }

    SECTION("Modifications")
    {
        SECTION("Rename")
        {
            out.setName("otherName");
            REQUIRE(out.name() == "otherName");
        }

        SECTION("Resize")
        {
            out.resize({2,2,2});
            REQUIRE(out.numberOfElements() == 8);
        }

        SECTION("Fields")
        {
            std::vector<std::string> structFields = structVar.fields();
            std::vector<std::string> arrayFields = in.fields();
            REQUIRE(structFields.size() == arrayFields.size());
            for (size_t i = 0; i < structVar.numberOfFields(); ++i)
            {
                REQUIRE(in.getFieldIndex(structFields[i]) == i);
                REQUIRE(structFields[i] == arrayFields[i]);
            }

            size_t numberOfFields = in.numberOfFields();
            REQUIRE(numberOfFields == arrayFields.size());

            REQUIRE(in.getFieldIndex("notExisting") == numberOfFields);

            in.addField("addedField");

            REQUIRE(in.numberOfFields() == numberOfFields + 1);
            REQUIRE(in.getFieldIndex("addedField") == numberOfFields);

            REQUIRE(in.addFields({"new1", "new2", "new3"}));
            REQUIRE(in.numberOfFields() == numberOfFields + 4);
            REQUIRE(in.getFieldIndex("new1") == numberOfFields +1);
            REQUIRE(in.getFieldIndex("new2") == numberOfFields +2);
            REQUIRE(in.getFieldIndex("new3") == numberOfFields +3);

        }

        SECTION("Set element")
        {
            matioCpp::Struct other = structVar;

            matioCpp::String newString("name", "otherContent");
            REQUIRE(other.setField(newString));
            REQUIRE(other("name").asString()() == "otherContent");
            other("element").asElement<int>() = 7;

            REQUIRE(in.setElement({0,1,1}, other));

            REQUIRE(in[{0,1,1}]("name").asString()() == "otherContent");
            REQUIRE(in[3]("element").asElement<int>() == 7);

        }

        SECTION("Clear")
        {
            out.clear();
            REQUIRE(out.numberOfElements() == 0);
            REQUIRE(out.numberOfFields() == 0);
        }
    }

    Mat_VarFree(matioVar);
}

