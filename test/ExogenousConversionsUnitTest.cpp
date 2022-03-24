/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

//#define MATIOCPP_NO_EIGEN
#include <catch2/catch.hpp>
#include <matioCpp/matioCpp.h>
#include <map>
#include <set>

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

        std::vector<std::string> stringVector = {"Huey", "Dewey", "Louie"};
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


