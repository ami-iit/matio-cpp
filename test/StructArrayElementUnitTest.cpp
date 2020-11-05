/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

#include <catch2/catch.hpp>
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

TEST_CASE("Struct array element")
{
    std::vector<matioCpp::Variable> data;
    data.emplace_back(matioCpp::Vector<double>("vector", 4));
    data.emplace_back(matioCpp::Element<int>("element", 1));
    data.emplace_back(matioCpp::MultiDimensionalArray<double>("array", {2,2}));
    data.emplace_back(matioCpp::String("name", "content"));

    matioCpp::Struct structVar("test", data);

    std::vector<matioCpp::Struct> structVector(6, structVar);

    matioCpp::StructArray array("test", {1,2,3}, structVector);

    SECTION("Constructors")
    {
        matioCpp::StructArrayElement<false> nonConst(1, &array);
        matioCpp::StructArrayElement<true> constEl(1, &array);

        //Copy
        matioCpp::StructArrayElement<false> copiedInNonConst(nonConst);
        matioCpp::StructArrayElement<true> copiedInConst(constEl);
        matioCpp::StructArrayElement<true> copiedInConstFromNonConst(nonConst);

        //Move
        matioCpp::StructArrayElement<false> movedInNonConst(std::move(nonConst));
        matioCpp::StructArrayElement<true> movedInConst(std::move(constEl));
        matioCpp::StructArrayElement<true> movedInConstFromNonConst(std::move(copiedInNonConst));

    }

    SECTION("Assignements")
    {
        matioCpp::StructArrayElement<false> nonConst(1, &array);
        matioCpp::StructArrayElement<true> constEl(1, &array);

        //Move
        matioCpp::StructArrayElement<false> movedInNonConst(1, &array);
        movedInNonConst = std::move(nonConst);
        matioCpp::StructArrayElement<true> movedInConst(1, &array);
        movedInConst = std::move(constEl);
        matioCpp::StructArrayElement<true> movedInConstFromNonConst(1, &array);
        matioCpp::StructArrayElement<false> copiedInNonConst(nonConst);
        movedInConstFromNonConst = std::move(copiedInNonConst);

    }

    SECTION("Fields")
    {
        matioCpp::StructArrayElement<false> element(1, &array);
        REQUIRE(element.numberOfFields() == array.numberOfFields());
        REQUIRE(element.numberOfFields() == 4);

        std::vector<std::string> elementsFields = element.fields();
        std::vector<std::string> arrayFields = array.fields();

        for (size_t i = 0; i < element.numberOfFields(); ++i)
        {
            REQUIRE(elementsFields[i] == arrayFields[i]);
            REQUIRE(element.isFieldExisting(arrayFields[i]));
            REQUIRE(element.getFieldIndex(arrayFields[i]) == i);
        }
    }

    SECTION("Modifications")
    {
        matioCpp::String modified("name", "modified");
        data.back() = modified;
        matioCpp::StructArrayElement<false> element(1, &array);

        structVar.setName("test1");
        checkSameStruct(element.asStruct(), structVar);

        REQUIRE(element.fromVectorOfVariables(data));

        REQUIRE(element.setField(1, matioCpp::Element<double>("test", 1.5)));
        REQUIRE(element[1].asElement<double>() == 1.5);

    }

}
