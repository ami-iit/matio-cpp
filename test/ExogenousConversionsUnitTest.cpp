/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

//#define MATIOCPP_NO_EIGEN
#include <catch2/catch_test_macros.hpp>
#include <matioCpp/matioCpp.h>
#include <map>
#include <set>
#include <algorithm>

template<typename Vector1, typename Vector2>
void checkSameVectors(const Vector1& a, const Vector2& b)
{
    REQUIRE(static_cast<size_t>(a.size()) == static_cast<size_t>(b.size()));

    for (size_t i = 0; i < static_cast<size_t>(a.size()); ++i)
    {
        REQUIRE(a[i] == b[i]);
    }
}

template<typename Matrix, typename type>
void checkSameMatrix(const Matrix& a, const matioCpp::MultiDimensionalArray<type>& b)
{
    REQUIRE(b.isValid());
    REQUIRE(b.dimensions().size() == 2);
    REQUIRE(static_cast<size_t>(a.rows()) == b.dimensions()(0));
    REQUIRE(static_cast<size_t>(a.cols()) == b.dimensions()(1));

    for (size_t i = 0; i < b.dimensions()(0); ++i)
    {
        for (size_t j = 0; j < b.dimensions()(1); ++j)
        {
            REQUIRE(a(i,j) == b({i,j}));
        }
    }
}

struct testStruct
{
    int i{1};
    double d{2.0};
    std::string s{"test"};
    std::vector<double> stdVec = {1.0, 2.0, 3.0, 4.0, 5.0};
    int* notSupported = nullptr;
    std::vector<std::string> stringVector = {"Huey", "Dewey", "Louie"};
    std::vector<bool> vecOfBool = {true, false, true};
};
VISITABLE_STRUCT(testStruct, i, d, s, stdVec, vecOfBool, stringVector);


struct nestedStruct
{
    using array_3f = std::array<float,3>;
    BEGIN_VISITABLES(nestedStruct);
    VISITABLE_DIRECT_INIT(array_3f, array, {1.0, 2.0, 3.0});
    VISITABLE(testStruct, s);
    END_VISITABLES;
    int* notSupported = nullptr;
};

#ifdef MATIOCPP_HAS_EIGEN

TEST_CASE("Eigen Conversions")
{

    SECTION("To Eigen")
    {
        std::vector<double> stdVec = {1.0, 2.0, 3.0, 4.0, 5.0};

        matioCpp::Vector<double> vector("vector", 5);
        vector = stdVec;

        Eigen::VectorXd eigenVec = matioCpp::to_eigen(vector);
        checkSameVectors(eigenVec, vector);

        Eigen::Matrix3f eigenMatrix;
        eigenMatrix << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;

        matioCpp::MultiDimensionalArray<float> matioCppMatrix("matrix", {3,3}, eigenMatrix.data());

        Eigen::MatrixXf toEigenMatrix = matioCpp::to_eigen(matioCppMatrix);
        checkSameMatrix(toEigenMatrix, matioCppMatrix);

        const auto& constMatioCppMatrix = matioCppMatrix;
        Eigen::MatrixXf toEigenMatrixConst = matioCpp::to_eigen(constMatioCppMatrix);
        checkSameMatrix(toEigenMatrixConst, constMatioCppMatrix);

        std::vector<float> tensor(12);
        for (size_t i = 0; i < 12; ++i)
        {
            tensor[i] = i + 1.0;
        }

        matioCpp::MultiDimensionalArray<float> matioCppMatrix2("matrix", { 2, 2, 3 }, tensor.data());

        /*
        So we have a tensor of the type
        | 1  3 | | 5 7 | |  9 11 |
        | 2  4 | | 6 8 | | 10 12 |
        */

        Eigen::MatrixXf expectedSlice(2, 2);
        expectedSlice << 1.0, 3.0,
                         2.0, 4.0;
        REQUIRE(expectedSlice.isApprox(matioCpp::to_eigen(matioCppMatrix2, { -1, -1, 0 }), 1e-5));


        expectedSlice.resize(2,3);
        expectedSlice << 2.0, 6.0, 10.0,
                         4.0, 8.0, 12.0;
        REQUIRE(expectedSlice.isApprox(matioCpp::to_eigen(matioCppMatrix2, {1, -1, -1 }), 1e-5));

        expectedSlice.resize(2, 3);
        expectedSlice << 3.0, 7.0, 11.0,
                         4.0, 8.0, 12.0;
        REQUIRE(expectedSlice.isApprox(matioCpp::to_eigen(matioCppMatrix2, { -1, 1, -1 }), 1e-5));

        expectedSlice.resize(2, 1);
        expectedSlice << 11.0,
                         12.0;
        REQUIRE(expectedSlice.isApprox(matioCpp::to_eigen(matioCppMatrix2, { -1, 1, 2 }), 1e-5));

        expectedSlice.resize(1, 1);
        expectedSlice << 8.0;
        REQUIRE(expectedSlice.isApprox(matioCpp::to_eigen(matioCppMatrix2, { 1, 1, 1 }), 1e-5));

        const auto& constMatioCppMatrix2 = matioCppMatrix2;
        expectedSlice.resize(2, 3);
        expectedSlice << 1.0, 5.0, 9.0,
                         3.0, 7.0, 11.0;
        REQUIRE(expectedSlice.isApprox(matioCpp::to_eigen(constMatioCppMatrix2, { 0, -1, -1 }), 1e-5));

    }

    SECTION("From Eigen")
    {
        Eigen::Matrix3f eigenMatrix;
        eigenMatrix << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;
        auto toMatioMatrix = matioCpp::make_variable("testMatrix", eigenMatrix);
        checkSameMatrix(eigenMatrix, toMatioMatrix);

        Eigen::Vector3i eigenVec;
        eigenVec << 2, 4, 6;
        auto toMatioEigenVec = matioCpp::make_variable("testEigen", eigenVec);
        checkSameVectors(eigenVec, toMatioEigenVec);
    }

}
#endif

TEST_CASE("Exogenous conversions")
{
    SECTION("make_variable_callable")
    {
        REQUIRE(matioCpp::is_make_variable_callable<std::vector<double>>::value);
        REQUIRE(matioCpp::is_make_variable_callable<std::array<float,3>>::value);
        REQUIRE(matioCpp::is_make_variable_callable<std::string>::value);
        REQUIRE(matioCpp::is_make_variable_callable<std::vector<bool>>::value);
        REQUIRE(matioCpp::is_make_variable_callable<double>::value);
        REQUIRE(matioCpp::is_make_variable_callable<bool>::value);
        REQUIRE(matioCpp::is_make_variable_callable<int>::value);
        REQUIRE(matioCpp::is_make_variable_callable<char>::value);
        REQUIRE(matioCpp::is_make_variable_callable<std::vector<std::string>>::value);
        REQUIRE(matioCpp::is_make_variable_callable<testStruct>::value);
    }

    SECTION("make_variable_callable")
    {
        REQUIRE(std::is_same<matioCpp::make_variable_output<std::vector<double>>::type, matioCpp::Vector<double>>::value);
        REQUIRE(std::is_same<matioCpp::make_variable_output<std::array<float,3>>::type, matioCpp::Vector<float>>::value);
        REQUIRE(std::is_same<matioCpp::make_variable_output<std::string>::type, matioCpp::String>::value);
        REQUIRE(std::is_same<matioCpp::make_variable_output<std::vector<bool>>::type, matioCpp::Vector<matioCpp::Logical>>::value);
        REQUIRE(std::is_same<matioCpp::make_variable_output<int>::type, matioCpp::Element<int>>::value);
        REQUIRE(std::is_same<matioCpp::make_variable_output<double>::type, matioCpp::Element<double>>::value);
        REQUIRE(std::is_same<matioCpp::make_variable_output<bool>::type, matioCpp::Element<matioCpp::Logical>>::value);
        REQUIRE(std::is_same<matioCpp::make_variable_output<char>::type, matioCpp::Element<char>>::value);
        REQUIRE(std::is_same<matioCpp::make_variable_output<std::vector<std::string>>::type, matioCpp::CellArray>::value);
        REQUIRE(std::is_same<matioCpp::make_variable_output<testStruct>::type, matioCpp::Struct>::value);
    }

    SECTION("Vector")
    {
        std::vector<double> stdVec = {1.0, 2.0, 3.0, 4.0, 5.0};
        auto toMatioVec = matioCpp::make_variable("test", stdVec);
        checkSameVectors(stdVec, toMatioVec);

        std::array<float,3> array = {1.0, 2.0, 3.0};
        auto toMatioArray = matioCpp::make_variable("test", array);
        checkSameVectors(array, toMatioArray);

        int classicalArray[] = {1, 2, 3};
        auto toMatioClassic = matioCpp::make_variable("test", matioCpp::make_span(classicalArray, 3));
        checkSameVectors(matioCpp::make_span(classicalArray, 3), toMatioClassic);

        std::string string("something");
        auto toMatioString = matioCpp::make_variable("name", string);
        REQUIRE(string == toMatioString());

        std::vector<bool> vecOfBool = {true, false, true};
        auto toVecofBool = matioCpp::make_variable("vecOfBool", vecOfBool);
        checkSameVectors(vecOfBool, toVecofBool);

        auto matioDouble = matioCpp::make_variable("double", 5.0);
        REQUIRE(matioDouble == 5.0);

        auto matioBool = matioCpp::make_variable("bool", true);
        REQUIRE(matioBool);

        auto matioInt = matioCpp::make_variable("int", 2);
        REQUIRE(matioInt == 2);

        auto matioChar = matioCpp::make_variable("char", 'f');
        REQUIRE(matioChar == 'f');

        std::vector<std::string> stringVector = {"Huey", "Dewey", "Louie", ""};
        auto matioCell = matioCpp::make_variable("stringVector", stringVector);
        for (size_t i = 0; i < stringVector.size(); ++i)
        {
            REQUIRE(matioCell[i].asString()() == stringVector[i]);
        }
    }

    SECTION("Struct")
    {
        std::unordered_map<std::string, int> unordered_map;
        unordered_map["one"] = 1;
        unordered_map["two"] = 2;
        unordered_map["three"] = 3;

        matioCpp::Struct toMatio = matioCpp::make_struct("map", unordered_map.begin(), unordered_map.end());

        REQUIRE(toMatio["one"].asElement<int>() == 1);
        REQUIRE(toMatio["two"].asElement<int>() == 2);
        REQUIRE(toMatio["three"].asElement<int>() == 3);

        testStruct s;
        matioCpp::Struct automaticStruct = matioCpp::make_variable("testStruct", s);
        REQUIRE(automaticStruct["i"].asElement<int>() == s.i);
        REQUIRE(automaticStruct["d"].asElement<double>() == s.d);
        REQUIRE(automaticStruct["s"].asString()() == s.s);
        checkSameVectors(automaticStruct["stdVec"].asVector<double>(), s.stdVec);
        checkSameVectors(automaticStruct["vecOfBool"].asVector<matioCpp::Logical>(), s.vecOfBool);

        for (size_t i = 0; i < s.stringVector.size(); ++i)
        {
            REQUIRE(automaticStruct["stringVector"].asCellArray()[i].asString()() == s.stringVector[i]);
        }

        auto fields = automaticStruct.fields();
        bool ok = std::find(fields.begin(), fields.end(), "notSupported") == fields.end();
        REQUIRE(ok);

        nestedStruct s2;
        matioCpp::Struct automaticNestedStruct = matioCpp::make_variable("testStruct2", s2);
        checkSameVectors(s2.array, automaticNestedStruct["array"].asVector<float>());
        REQUIRE(automaticNestedStruct["s"].asStruct()["i"].asElement<int>() == s2.s.i);
        REQUIRE(automaticNestedStruct["s"].asStruct()["d"].asElement<double>() == s2.s.d);
        REQUIRE(automaticNestedStruct["s"].asStruct()["s"].asString()() == s2.s.s);
        checkSameVectors(automaticNestedStruct["s"].asStruct()["stdVec"].asVector<double>(), s2.s.stdVec);
        checkSameVectors(automaticNestedStruct["s"].asStruct()["vecOfBool"].asVector<matioCpp::Logical>(), s2.s.vecOfBool);

        for (size_t i = 0; i < s2.s.stringVector.size(); ++i)
        {
            REQUIRE(automaticNestedStruct["s"].asStruct()["stringVector"].asCellArray()[i].asString()() == s2.s.stringVector[i]);
        }

        auto fields2 = automaticNestedStruct.fields();
        ok = std::find(fields2.begin(), fields2.end(), "notSupported") == fields2.end();
        REQUIRE(ok);
    }

    SECTION("Cell Array")
    {
        std::map<std::string, char> map;
        map["a"] = 'a';
        map["b"] = 'b';
        map["c"] = 'c';

        matioCpp::CellArray toMatioCell = matioCpp::make_cell_array("mapAsCell", map.begin(), map.end());
        REQUIRE(toMatioCell[0].asElement<char>() == 'a');
        REQUIRE(toMatioCell[0].name() == "a");
        REQUIRE(toMatioCell[1].asElement<char>() == 'b');
        REQUIRE(toMatioCell[1].name() == "b");
        REQUIRE(toMatioCell[2].asElement<char>() == 'c');
        REQUIRE(toMatioCell[2].name() == "c");

        std::set<std::string> set({"Huey", "Dewey", "Louie"});
        auto toMatioCellFromSet = matioCpp::make_cell_array("setAsCell", set.begin(), set.end());
        REQUIRE(toMatioCellFromSet[0].asString()() == "Dewey");
        REQUIRE(toMatioCellFromSet[1].asString()() == "Huey");
        REQUIRE(toMatioCellFromSet[2].asString()() == "Louie");
    }
}



