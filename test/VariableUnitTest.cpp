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
    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, static_cast<int>(dimensions.size()), dimensions.data(), vec.data(), 0);
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
    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, static_cast<int>(dimensions.size()), dimensions.data(), vec.data(), 0);
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
    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, static_cast<int>(dimensions.size()), dimensions.data(), vec.data(), 0);
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
        matioCpp::Variable c;
        c = a;
        checkSameVariable(a,c);
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
        matioCpp::Variable c;
        c = std::move(b);
        checkVariable(c, "test", matioCpp::VariableType::Vector,
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

    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_INT64, static_cast<int>(dimensions.size()), dimensions.data(), &matioComplexSplit, MAT_F_COMPLEX);
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
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, static_cast<int>(dimensions.size()), dimensions.data(), vec.data(), 0);
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
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, static_cast<int>(dimensions.size()), dimensions.data(), vec.data(), 0);
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
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, static_cast<int>(dimensions.size()), dimensions.data(), &input, 0);
        REQUIRE(matioVar);
        matioCpp::Variable sharedVar((matioCpp::SharedMatvar(matioVar)));

        matioCpp::Element<double> asElement = sharedVar.asElement<double>();

        REQUIRE(asElement == 3.14);
    }

    SECTION("To String")
    {
        char inputString[] = "test_string";
        std::vector<size_t> dimensions = {1, strlen(inputString)};
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_CHAR, matio_types::MAT_T_UTF8, static_cast<int>(dimensions.size()), dimensions.data(), inputString, 0);
        REQUIRE(matioVar);
        matioCpp::Variable sharedVar((matioCpp::SharedMatvar(matioVar)));

        matioCpp::String asElement = sharedVar.asString();

        REQUIRE(asElement() == "test_string");
    }

    SECTION("To String16")
    {
        char16_t inputString[] = u"test_string";
        std::vector<size_t> dimensions = {1, strlen("test_string")};
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_CHAR, matio_types::MAT_T_UINT16, static_cast<int>(dimensions.size()), dimensions.data(), inputString, 0);
        REQUIRE(matioVar);
        matioCpp::Variable sharedVar((matioCpp::SharedMatvar(matioVar)));

        matioCpp::String16 outputString16 = sharedVar.asString16();
        REQUIRE(outputString16() == u"test_string");
    }

    SECTION("To String32")
    {
        char32_t inputString[] = U"test_string";
        std::vector<size_t> dimensions = {1, strlen("test_string")};
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_CHAR, matio_types::MAT_T_UINT32, static_cast<int>(dimensions.size()), dimensions.data(), inputString, 0);
        REQUIRE(matioVar);
        matioCpp::Variable sharedVar((matioCpp::SharedMatvar(matioVar)));

        matioCpp::String32 outputString32 = sharedVar.asString32();
        REQUIRE(outputString32() == U"test_string");
    }


    SECTION("To Cell Array")
    {
        std::vector<matvar_t*> pointers;
        std::vector<size_t> dimensions = {1,2,3};
        pointers.emplace_back(Mat_VarDuplicate(matioCpp::Vector<double>("vector").toMatio(), 1));
        pointers.emplace_back(Mat_VarDuplicate(matioCpp::Element<int>("element").toMatio(), 1));
        pointers.emplace_back(nullptr);
        pointers.emplace_back(Mat_VarDuplicate(matioCpp::MultiDimensionalArray<double>("array").toMatio(), 1));
        pointers.emplace_back(Mat_VarDuplicate(matioCpp::String("name", "content").toMatio(), 1));
        pointers.emplace_back(Mat_VarDuplicate(matioCpp::CellArray("otherCell").toMatio(), 1));
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_CELL, matio_types::MAT_T_CELL, static_cast<int>(dimensions.size()), dimensions.data(), pointers.data(), 0);
        REQUIRE(matioVar);

        matioCpp::Variable sharedVar((matioCpp::SharedMatvar(matioVar)));

        matioCpp::CellArray cellArray = sharedVar.asCellArray();

        matioCpp::String anotherString("another", "anotherString");

        REQUIRE(cellArray.setElement({0,0,2}, anotherString));
        REQUIRE(cellArray({0,0,2}).asString()() == "anotherString");

        matvar_t* cell = Mat_VarGetCell(matioVar, static_cast<int>(cellArray.rawIndexFromIndices({0,0,2})));
        std::string expected = "anotherString";
        REQUIRE(strncmp((char*)cell->data, expected.c_str(), expected.size()) == 0);
    }

    SECTION("To Struct")
    {
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

        matioCpp::Variable sharedVar((matioCpp::SharedMatvar(matioVar)));

        matioCpp::Struct structVar = sharedVar.asStruct();

        matioCpp::String anotherString("name", "anotherString");

        REQUIRE(structVar.setField(structVar.getFieldIndex("name"), anotherString));
        REQUIRE(structVar("name").asString()() == "anotherString");

        matvar_t* field = Mat_VarGetStructFieldByIndex(matioVar, structVar.getFieldIndex("name"), 0);
        std::string expected = "anotherString";
        REQUIRE(strncmp((char*)field->data, expected.c_str(), expected.size()) == 0);
    }

    SECTION("To Struct Array")
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

        matioCpp::Variable sharedVar((matioCpp::SharedMatvar(matioVar)));

        matioCpp::StructArray structArray = sharedVar.asStructArray();
        REQUIRE(structArray.isValid());

        matioCpp::String anotherString("name", "anotherContent");
        REQUIRE(structArray({1,1}).setField(anotherString));

        matvar_t* field = Mat_VarGetStructFieldByIndex(matioVar, structArray.getFieldIndex("name"), structArray.rawIndexFromIndices({1,1}));
        std::string expected = "anotherContent";
        REQUIRE(strncmp((char*)field->data, expected.c_str(), expected.size()) == 0);

    }
}

TEST_CASE("operator[](string)")
{
    std::vector<size_t> dimensions = {1, 1};
    std::vector<matvar_t*> pointers;
    pointers.emplace_back(Mat_VarDuplicate(matioCpp::Vector<double>("vector").toMatio(), 1));
    pointers.emplace_back(Mat_VarDuplicate(matioCpp::Element<int>("element").toMatio(), 1));
    pointers.emplace_back(Mat_VarDuplicate(matioCpp::MultiDimensionalArray<double>("array").toMatio(), 1));
    pointers.emplace_back(Mat_VarDuplicate(matioCpp::String("name", "content").toMatio(), 1));
    pointers.emplace_back(Mat_VarDuplicate(matioCpp::Struct("otherStruct", { matioCpp::String("name", "anotherContent") }).toMatio(), 1));
    pointers.emplace_back(nullptr);

    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_STRUCT, matio_types::MAT_T_STRUCT, 2, dimensions.data(), pointers.data(), 0);
    REQUIRE(matioVar);

    matioCpp::Variable sharedVar((matioCpp::SharedMatvar(matioVar)));

    REQUIRE(sharedVar["name"].asString()() == "content");

    const matioCpp::Variable& constRef = sharedVar;
    REQUIRE(constRef["name"].asString()() == "content");

    REQUIRE(sharedVar["otherStruct"]["name"].asString()() == "anotherContent");
}
