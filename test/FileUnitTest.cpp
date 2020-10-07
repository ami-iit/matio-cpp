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
#include <matioCpp/matioCpp.h>
#include "MatFolderPath.h"

TEST_CASE("Default constructor / open /close file")
{
    matioCpp::File input;

    REQUIRE(input.open(getMatPath("input.mat")));
    REQUIRE(input.isOpen());

    input.close();
    REQUIRE_FALSE(input.isOpen());
}

TEST_CASE("Constructors"){

    matioCpp::File input(getMatPath("input.mat"));

    REQUIRE(input.isOpen());

    matioCpp::File other(std::move(input));
    REQUIRE(other.isOpen());
}


TEST_CASE("File properties")
{
    matioCpp::File input;

    REQUIRE(input.open(getMatPath("input.mat")));
    REQUIRE(input.isOpen());

    REQUIRE(input.name() == getMatPath("input.mat"));
    REQUIRE(input.version() == matioCpp::FileVersion::MAT5);
    REQUIRE(input.mode() == matioCpp::FileMode::ReadAndWrite);


    input.close();
    REQUIRE(input.open(getMatPath("input.mat"), matioCpp::FileMode::ReadOnly));
    REQUIRE(input.isOpen());

    REQUIRE(input.name() == getMatPath("input.mat"));
    REQUIRE(input.version() == matioCpp::FileVersion::MAT5);
    REQUIRE(input.mode() == matioCpp::FileMode::ReadOnly);
}

TEST_CASE("Variable names")
{
    matioCpp::File input;

    REQUIRE(input.open(getMatPath("input.mat")));
    REQUIRE(input.isOpen());

    std::vector<std::string> names = input.variableNames();
    REQUIRE(names[0] == "cell_array");
    REQUIRE(names[1] == "cell_matrix");
    REQUIRE(names[2] == "double");
    REQUIRE(names[3] == "int");
    REQUIRE(names[4] == "matrix");
    REQUIRE(names[5] == "string");
    REQUIRE(names[6] == "struct");
    REQUIRE(names[7] == "struct_array");
    REQUIRE(names[8] == "vector");
}

TEST_CASE("Read")
{
    matioCpp::File input(getMatPath("input.mat"));

    REQUIRE(input.isOpen());

    matioCpp::Variable cellArray = input.read("cell_array");
    REQUIRE(cellArray.isValid());

    matioCpp::Variable cellMatrix = input.read("cell_matrix");
    REQUIRE(cellMatrix.isValid());

    matioCpp::Element<double> doubleVar = input.read("double").asElement<double>();
    REQUIRE(doubleVar.isValid());
    REQUIRE(doubleVar() == 3.14);

    matioCpp::Element<int> intVar = input.read("int").asElement<int>();
    REQUIRE(intVar.isValid());
    REQUIRE(intVar() == 5);

    matioCpp::MultiDimensionalArray<double> matrix = input.read("matrix").asMultiDimensionalArray<double>();
    REQUIRE(matrix.isValid());
    REQUIRE(matrix.dimensions().size() == 3);
    REQUIRE(matrix.dimensions()[0] == 3);
    REQUIRE(matrix.dimensions()[1] == 3);
    REQUIRE(matrix.dimensions()[2] == 3);

    for (size_t i = 0; i < 3; ++i)
    {
        for (size_t j = 0; j < 3; ++j)
        {
            for (size_t k = 0; k < 3; ++k)
            {
                REQUIRE(matrix({i,j,k}) == i + 3*j + 9*k);
            }
        }
    }

    matioCpp::String string = input.read("string").asString();
    REQUIRE(string.isValid());
    REQUIRE(string() == "test");

    matioCpp::Variable structVar = input.read("struct");
    REQUIRE(structVar.isValid());

    matioCpp::Variable structArray = input.read("struct_array");
    REQUIRE(structArray.isValid());

    matioCpp::Vector<double> vector = input.read("vector").asVector<double>();
    REQUIRE(vector.isValid());
    REQUIRE(vector.size() == 6);
    for (size_t i = 0; i < 5; ++i)
    {
        REQUIRE(vector[i] == i+1);
    }
}

TEST_CASE("Create and delete file")
{
    matioCpp::File::Delete("test.mat");
    matioCpp::File test;
    REQUIRE_FALSE(test.open("test.mat", matioCpp::FileMode::ReadOnly));
    matioCpp::File newFile = matioCpp::File::Create("test.mat");
    REQUIRE(newFile.isOpen());
    REQUIRE(newFile.variableNames().size() == 0);
    REQUIRE(matioCpp::File::Delete("test.mat"));
    REQUIRE_FALSE(test.open("test.mat", matioCpp::FileMode::ReadOnly));
}

TEST_CASE("Write")
{
    matioCpp::File file = matioCpp::File::Create("test.mat");

    matioCpp::Element<double> doubleVar("double");
    doubleVar = 3.14;
    REQUIRE(file.write(doubleVar));
    REQUIRE(file.read("double").asElement<double>()() == 3.14);

    matioCpp::Element<int> intVar("int", 5);
    REQUIRE(file.write(intVar));
    REQUIRE(file.read("int").asElement<int>()() == 5);

    matioCpp::MultiDimensionalArray<double> matrixInput("matrix", {3,3,3});
    for (size_t i = 0; i < 3; ++i)
    {
        for (size_t j = 0; j < 3; ++j)
        {
            for (size_t k = 0; k < 3; ++k)
            {
                matrixInput({i,j,k}) = i + 3*j + 9*k;
            }
        }
    }
    REQUIRE(file.write(matrixInput));

    matioCpp::MultiDimensionalArray<double> matrix = file.read("matrix").asMultiDimensionalArray<double>();
    REQUIRE(matrix.isValid());
    REQUIRE(matrix.dimensions().size() == 3);
    REQUIRE(matrix.dimensions()[0] == 3);
    REQUIRE(matrix.dimensions()[1] == 3);
    REQUIRE(matrix.dimensions()[2] == 3);

    for (size_t i = 0; i < 3; ++i)
    {
        for (size_t j = 0; j < 3; ++j)
        {
            for (size_t k = 0; k < 3; ++k)
            {
                REQUIRE(matrix({i,j,k}) == i + 3*j + 9*k);
            }
        }
    }

    matioCpp::String inputString("string", "test");
    REQUIRE(file.write(inputString));
    REQUIRE(file.read("string").asString()() == "test");

    std::vector<double> data({1, 2, 3, 4, 5, 6});
    matioCpp::Vector<double> vectorInput("vector", data);
    REQUIRE(file.write(vectorInput));
    matioCpp::Vector<double> vector = file.read("vector").asVector<double>();
    REQUIRE(vector.isValid());
    REQUIRE(vector.size() == 6);
    for (size_t i = 0; i < 5; ++i)
    {
        REQUIRE(vector[i] == i+1);
    }
}
