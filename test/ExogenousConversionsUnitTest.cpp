/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

//#define MATIOCPP_NO_EIGEN
#include <catch2/catch.hpp>
#include <matioCpp/matioCpp.h>

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
        auto toMatioMatrix = matioCpp::make_variable(eigenMatrix, "testMatrix");
        checkSameMatrix(eigenMatrix, toMatioMatrix);

        Eigen::Vector3i eigenVec;
        eigenVec << 2, 4, 6;
        auto toMatioEigenVec = matioCpp::make_variable(eigenVec, "testEigen");
        checkSameVectors(eigenVec, toMatioEigenVec);
    }

}
#endif

TEST_CASE("Exogenous conversions")
{
    SECTION("Vector")
    {
        std::vector<double> stdVec = {1.0, 2.0, 3.0, 4.0, 5.0};
        auto toMatioVec = matioCpp::make_variable(stdVec, "test");
        checkSameVectors(stdVec, toMatioVec);

        std::array<float,3> array = {1.0, 2.0, 3.0};
        auto toMatioArray = matioCpp::make_variable(array, "test");
        checkSameVectors(array, toMatioArray);

        int classicalArray[] = {1, 2, 3};
        auto toMatioClassic = matioCpp::make_variable(matioCpp::make_span(classicalArray, 3), "test");
        checkSameVectors(matioCpp::make_span(classicalArray, 3), toMatioClassic);

        std::string string("something");
        auto toMatioString = matioCpp::make_variable(string, "name");
        REQUIRE(string == toMatioString());

        std::vector<bool> vecOfBool = {true, false, true};
        auto toVecofBool = matioCpp::make_variable(vecOfBool, "vecOfBool");
        checkSameVectors(vecOfBool, toVecofBool);

        auto matioDouble = matioCpp::make_variable(5.0, "double");
        REQUIRE(matioDouble == 5.0);

        auto matioBool = matioCpp::make_variable(true, "bool");
        REQUIRE(matioBool);

        auto matioInt = matioCpp::make_variable(2, "bool");
        REQUIRE(matioInt == 2);

        auto matioChar = matioCpp::make_variable('f', "char");
        REQUIRE(matioChar == 'f');

        std::vector<std::string> stringVector = {"Huey", "Dewey", "Louie"};
        auto matioCell = matioCpp::make_variable(stringVector, "stringVector");
        for (size_t i = 0; i < stringVector.size(); ++i)
        {
            REQUIRE(matioCell[i].asString()() == stringVector[i]);
        }
    }
}



