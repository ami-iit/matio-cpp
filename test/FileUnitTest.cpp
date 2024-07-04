/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

#include <catch2/catch_test_macros.hpp>
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

TEST_CASE("Move assignement")
{
    matioCpp::File input(getMatPath("input.mat"));

    REQUIRE(input.isOpen());

    matioCpp::File other;

    other = std::move(input);

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
    REQUIRE(names[0] == "boolean");
    REQUIRE(names[1] == "cell_array");
    REQUIRE(names[2] == "cell_matrix");
    REQUIRE(names[3] == "double");
    REQUIRE(names[4] == "int");
    REQUIRE(names[5] == "matrix");
    REQUIRE(names[6] == "matrix_bool");
    REQUIRE(names[7] == "string");
    REQUIRE(names[8] == "struct");
    REQUIRE(names[9] == "struct_array");
    REQUIRE(names[10] == "vector");
    REQUIRE(names[11] == "vector_bool");
}

TEST_CASE("Read")
{
    REQUIRE(matioCpp::File::Exists(getMatPath("input.mat")));
    matioCpp::File input(getMatPath("input.mat"));

    REQUIRE(input.isOpen());

    matioCpp::Element<matioCpp::Logical> boolean = input.read("boolean").asElement<matioCpp::Logical>();
    REQUIRE(boolean.isValid());
    REQUIRE(boolean);

    matioCpp::CellArray cellArray = input.read("cell_array").asCellArray();
    REQUIRE(cellArray.isValid());
    REQUIRE(cellArray.dimensions().size() == 2);
    REQUIRE(cellArray.dimensions()[0] == 1);
    REQUIRE(cellArray.dimensions()[1] == 3);
    REQUIRE(cellArray({0,0}).asElement<int>()() == 1);
    REQUIRE(cellArray({0,1}).asElement<char>()() == 'a');
    REQUIRE(cellArray({0,2}).asElement<double>()() == 3.14);

    matioCpp::CellArray cellMatrix = input.read("cell_matrix").asCellArray();
    REQUIRE(cellMatrix.isValid());
    REQUIRE(cellMatrix.dimensions().size() == 2);
    REQUIRE(cellMatrix.dimensions()[0] == 2);
    REQUIRE(cellMatrix.dimensions()[1] == 2);
    matioCpp::CellArray cellElement = cellMatrix({0,0}).asCellArray();
    REQUIRE(cellElement.isValid());
    REQUIRE(cellMatrix({0,1}).asElement<int>()() == 7);
    REQUIRE(cellMatrix({1,1}).asElement<double>()() == 3.14);

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

    matioCpp::MultiDimensionalArray<matioCpp::Logical> logicalMatrix = input.read("matrix_bool").asMultiDimensionalArray<matioCpp::Logical>();
    REQUIRE(logicalMatrix.isValid());
    REQUIRE(logicalMatrix.dimensions()(0) == 2);
    REQUIRE(logicalMatrix.dimensions()(1) == 4);
    REQUIRE(logicalMatrix({0,0}));
    REQUIRE_FALSE(logicalMatrix({0,1}));
    REQUIRE(logicalMatrix({0,2}));
    REQUIRE_FALSE(logicalMatrix({0,3}));
    REQUIRE(logicalMatrix({1,0}));
    REQUIRE_FALSE(logicalMatrix({1,1}));
    REQUIRE(logicalMatrix({1,2}));
    REQUIRE_FALSE(logicalMatrix({1,3}));

    matioCpp::String inputString = input.read("string").asString();
    REQUIRE(inputString.isValid());
    std::string stringValue = inputString();
    bool ok = stringValue == "test";
    if (!ok)
    {
        std::cerr << stringValue << "!= test" << std::endl;
    }
    REQUIRE(ok); //REQUIRE(inputString() == "test") is failing in some systems. (https://github.com/ami-iit/matio-cpp/issues/28)

    matioCpp::Struct structVar = input.read("struct").asStruct();
    REQUIRE(structVar.isValid());
    REQUIRE(structVar.numberOfFields() == 3);
    REQUIRE(structVar.isFieldExisting("string"));
    REQUIRE(structVar.isFieldExisting("double"));
    REQUIRE(structVar.isFieldExisting("int"));
    std::vector<std::string> fields = structVar.fields();
    REQUIRE(fields[0] == "string");
    REQUIRE(fields[1] == "double");
    REQUIRE(fields[2] == "int");
    REQUIRE(structVar("double").asElement<double>()() == 1.7);
    REQUIRE(structVar("int").asElement<int>()() == 43);

    matioCpp::StructArray structArray = input.read("struct_array").asStructArray();
    REQUIRE(structArray.isValid());
    REQUIRE(structArray.numberOfFields() == 3);
    REQUIRE(structArray.isFieldExisting("string"));
    REQUIRE(structArray.isFieldExisting("double"));
    REQUIRE(structArray.isFieldExisting("int"));
    REQUIRE(structArray.numberOfElements() == 3);
    REQUIRE(structArray({0,1})("double").asElement<double>()() == 1.7);
    REQUIRE(structArray({0,2})("int").asElement<int>()() == 43);


    matioCpp::Vector<double> vector = input.read("vector").asVector<double>();
    REQUIRE(vector.isValid());
    REQUIRE(vector.size() == 6);
    for (size_t i = 0; i < 5; ++i)
    {
        REQUIRE(vector[i] == i+1);
    }

    matioCpp::Vector<matioCpp::Logical> logicalVector = input.read("vector_bool").asVector<matioCpp::Logical>();
    REQUIRE(logicalVector.isValid());
    REQUIRE(logicalVector.size() == 4);
    REQUIRE(logicalVector(0));
    REQUIRE_FALSE(logicalVector(1));
    REQUIRE(logicalVector(2));
    REQUIRE_FALSE(logicalVector(3));


    matioCpp::Variable notExisting = input.read("not_existing");
    REQUIRE_FALSE(notExisting.isValid());

}

TEST_CASE("Create and delete file")
{
    matioCpp::File::Delete("test.mat");
    matioCpp::File test;
    REQUIRE_FALSE(test.open("test.mat", matioCpp::FileMode::ReadOnly));
    matioCpp::File dummy = matioCpp::File::Create("/shouldFail/test.mat");
    REQUIRE_FALSE(dummy.isOpen());
    matioCpp::File newFile = matioCpp::File::Create("test.mat");
    REQUIRE(newFile.isOpen());
    REQUIRE(newFile.variableNames().size() == 0);
    newFile.close();
    REQUIRE(matioCpp::File::Delete("test.mat"));
    REQUIRE_FALSE(matioCpp::File::Exists("test.mat"));
}

TEST_CASE("Write version 5")
{
    matioCpp::File file = matioCpp::File::Create("test.mat", matioCpp::FileVersion::MAT5);

    matioCpp::Element<double> doubleVar("double");
    doubleVar = 3.14;
    REQUIRE(file.write(doubleVar));
    REQUIRE(file.read("double").asElement<double>()() == 3.14);

    matioCpp::Element<int> intVar("int", 5);
    REQUIRE(file.write(intVar));
    REQUIRE(file.read("int").asElement<int>()() == 5);

    matioCpp::Element<matioCpp::Logical> logicalVar("boolean", true);
    REQUIRE(file.write(logicalVar));
    REQUIRE(file.read("boolean").asElement<matioCpp::Logical>());

    matioCpp::MultiDimensionalArray<double> matrixInput("matrix", {3,3,3});
    for (size_t i = 0; i < 3; ++i)
    {
        for (size_t j = 0; j < 3; ++j)
        {
            for (size_t k = 0; k < 3; ++k)
            {
                matrixInput({i,j,k}) = i + 3.0*j + 9.0*k;
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
    REQUIRE(file.read("string").asString()() == "test"); //In a MAT5, a string should be always retrieved with type UTF8

    std::vector<double> data({1, 2, 3, 4, 5, 6});
    matioCpp::Vector<double> vectorInput("vector", data);
    REQUIRE(file.write(vectorInput, matioCpp::Compression::zlib));
    matioCpp::Vector<double> vector = file.read("vector").asVector<double>();
    REQUIRE(vector.isValid());
    REQUIRE(vector.size() == 6);
    for (size_t i = 0; i < 5; ++i)
    {
        REQUIRE(vector[i] == i+1);
    }

    std::vector<bool> boolData({true, false, true, false});
    matioCpp::Vector<matioCpp::Logical> boolVectorInput("bool_vector", boolData);
    REQUIRE(file.write(boolVectorInput));
    matioCpp::Vector<matioCpp::Logical> boolVector = file.read("bool_vector").asVector<matioCpp::Logical>();
    REQUIRE(boolVector.isValid());
    REQUIRE(boolVector.size() == 4);
    REQUIRE(boolVector(0));
    REQUIRE_FALSE(boolVector(1));
    REQUIRE(boolVector(2));
    REQUIRE_FALSE(boolVector(3));


    std::vector<matioCpp::Variable> dataCell;
    dataCell.emplace_back(matioCpp::Vector<double>("vector", 4));
    dataCell.emplace_back(matioCpp::Element<int>("element", 3));
    dataCell.emplace_back(matioCpp::String("name", "content"));
    dataCell.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
    dataCell.emplace_back(matioCpp::String("otherString", "content"));
    dataCell.emplace_back(matioCpp::CellArray("otherCell"));

    matioCpp::CellArray cellArray("cellArray", {1,2,3}, dataCell);
    REQUIRE(file.write(cellArray));
    matioCpp::CellArray readCellArray = file.read("cellArray").asCellArray();

    REQUIRE(readCellArray.isValid());
    REQUIRE(readCellArray({0,0,2}).asString()() == "content");

    std::vector<matioCpp::Variable> dataVector;
    dataVector.emplace_back(matioCpp::Vector<double>("vector", 4));
    dataVector.emplace_back(matioCpp::Element<int>("element", 3));
    dataVector.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
    dataVector.emplace_back(matioCpp::String("name", "content"));
    dataVector.emplace_back(matioCpp::Struct("otherStruct"));

    matioCpp::Struct structVar("struct", dataVector);
    REQUIRE(file.write(structVar));
    matioCpp::Struct readStructVar = file.read("struct").asStruct();

    REQUIRE(readStructVar.isValid());
    REQUIRE(readStructVar("element").asElement<int>()() == 3);

    std::vector<matioCpp::Struct> structVector(6, structVar);
    matioCpp::StructArray structArray("structArray", {1,2,3}, structVector);
    REQUIRE(file.write(structArray));
    matioCpp::StructArray readStructArrayVar = file.read("structArray").asStructArray();

    REQUIRE(readStructArrayVar.isValid());
    REQUIRE(readStructArrayVar(0)("element").asElement<int>()() == 3);

    REQUIRE_FALSE(file.write(matioCpp::Element<double>("Should Fail", 1.0)));

    matioCpp::StructArray empty("emptyStructArray", {2,2});
    empty.addField("empty field");
    REQUIRE(file.write(empty));
}

TEST_CASE("Batch write")
{
    matioCpp::File::Delete("testBatch.mat");
    matioCpp::File file = matioCpp::File::Create("testBatch.mat");

    std::vector<matioCpp::Variable> dataVector;
    dataVector.emplace_back(matioCpp::Vector<double>("vector", 4));
    dataVector.emplace_back(matioCpp::Element<int>("element", 3));
    dataVector.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
    dataVector.emplace_back(matioCpp::String("name", "content"));

    REQUIRE(file.write(dataVector.begin(), dataVector.end()));


    matioCpp::File::Delete("testBatchMap.mat");
    matioCpp::File file2 = matioCpp::File::Create("testBatchMap.mat");

    std::unordered_map<std::string, matioCpp::Variable> dataMap;
    dataMap.insert(std::make_pair("vector", matioCpp::Vector<double>("vector", 4)));
    dataMap.insert(std::make_pair("element", matioCpp::Element<int>("element", 3)));
    dataMap.insert(std::make_pair("array", matioCpp::MultiDimensionalArray<double>("array")));
    dataMap.insert(std::make_pair("name", matioCpp::String("name", "content")));

    REQUIRE(file2.write(dataMap.cbegin(), dataMap.cend()));
}

#if !defined(_MSC_VER) || MATIO_VERSION >= 1519 //Reading from a MAT7.3 file on Windows with a matio version lower than 1.5.19 causes segfaults

TEST_CASE("Write version 7.3")
{
    matioCpp::File::Delete("test73.mat");
    matioCpp::File file = matioCpp::File::Create("test73.mat", matioCpp::FileVersion::MAT7_3);

    matioCpp::Element<double> doubleVar("double");
    doubleVar = 3.14;
    REQUIRE(file.write(doubleVar));
    REQUIRE(file.read("double").asElement<double>()() == 3.14);

    matioCpp::Element<int> intVar("int", 5);
    REQUIRE(file.write(intVar));
    REQUIRE(file.read("int").asElement<int>()() == 5);

    matioCpp::Element<matioCpp::Logical> logicalVar("boolean", true);
    REQUIRE(file.write(logicalVar));
    REQUIRE(file.read("boolean").asElement<matioCpp::Logical>());

    matioCpp::MultiDimensionalArray<double> matrixInput("matrix", {3,3,3});
    for (size_t i = 0; i < 3; ++i)
    {
        for (size_t j = 0; j < 3; ++j)
        {
            for (size_t k = 0; k < 3; ++k)
            {
                matrixInput({i,j,k}) = i + 3.0*j + 9.0*k;
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
    matioCpp::Variable readString = file.read("string");
    if ((readString.valueType() == matioCpp::ValueType::UTF8) || (readString.valueType() == matioCpp::ValueType::UINT8))
    {
        REQUIRE(readString.asString()() == "test");
    }
    else //In some systems, the string ,may be read with type UINT16
    {
        REQUIRE(readString.asString16()() == u"test");
    }

    std::vector<double> data({1, 2, 3, 4, 5, 6});
    matioCpp::Vector<double> vectorInput("vector", data);
    REQUIRE(file.write(vectorInput, matioCpp::Compression::zlib));
    matioCpp::Vector<double> vector = file.read("vector").asVector<double>();
    REQUIRE(vector.isValid());
    REQUIRE(vector.size() == 6);
    for (size_t i = 0; i < 5; ++i)
    {
        REQUIRE(vector[i] == i+1);
    }

    std::vector<bool> boolData({true, false, true, false});
    matioCpp::Vector<matioCpp::Logical> boolVectorInput("bool_vector", boolData);
    REQUIRE(file.write(boolVectorInput));
    matioCpp::Vector<matioCpp::Logical> boolVector = file.read("bool_vector").asVector<matioCpp::Logical>();
    REQUIRE(boolVector.isValid());
    REQUIRE(boolVector.size() == 4);
    REQUIRE(boolVector(0));
    REQUIRE_FALSE(boolVector(1));
    REQUIRE(boolVector(2));
    REQUIRE_FALSE(boolVector(3));


    std::vector<matioCpp::Variable> dataCell;
    dataCell.emplace_back(matioCpp::Vector<double>("vector", 4));
    dataCell.emplace_back(matioCpp::Element<int>("element", 3));
    dataCell.emplace_back(matioCpp::String("name", "content"));
    dataCell.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
    dataCell.emplace_back(matioCpp::String("otherString", "content"));
    dataCell.emplace_back(matioCpp::CellArray("otherCell"));

    matioCpp::CellArray cellArray("cellArray", {1,2,3}, dataCell);
    REQUIRE(file.write(cellArray));
    matioCpp::CellArray readCellArray = file.read("cellArray").asCellArray();

    REQUIRE(readCellArray.isValid());
    REQUIRE(readCellArray({0,1,0}).asElement<int>() == 3);

    std::vector<matioCpp::Variable> dataVector;
    dataVector.emplace_back(matioCpp::Vector<double>("vector", 4));
    dataVector.emplace_back(matioCpp::Element<int>("element", 3));
    dataVector.emplace_back(matioCpp::MultiDimensionalArray<double>("array"));
    dataVector.emplace_back(matioCpp::String("name", "content"));
    dataVector.emplace_back(matioCpp::Struct("otherStruct"));

    matioCpp::Struct structVar("struct", dataVector);
    REQUIRE(file.write(structVar));
    matioCpp::Struct readStructVar = file.read("struct").asStruct();

    REQUIRE(readStructVar.isValid());
    REQUIRE(readStructVar("element").asElement<int>()() == 3);

    std::vector<matioCpp::Struct> structVector(6, structVar);
    matioCpp::StructArray structArray("structArray", {1,2,3}, structVector);
    REQUIRE(file.write(structArray, matioCpp::Compression::zlib));
//    matioCpp::StructArray readStructArrayVar = file.read("structArray").asStructArray(); //Disabling this for now. Reading a struct array from a mat73 file may result in a memory leak. This has been fixed in new versions of matio. See https://github.com/tbeu/matio/commit/b783c3e234ceac1d933266707e453b085f1112c6
//    REQUIRE(readStructArrayVar.isValid());
//    REQUIRE(readStructArrayVar(0)("element").asElement<int>()() == 3);
}
#endif

TEST_CASE("Write version 4")
{
    //MAT_4 version supports only numeric variables of rank at most 2 (hence only element, vectors and matrices).
    matioCpp::File::Delete("test4.mat");
    matioCpp::File file = matioCpp::File::Create("test4.mat", matioCpp::FileVersion::MAT4);

    matioCpp::Element<double> doubleVar("double");
    doubleVar = 3.14;
    REQUIRE(file.write(doubleVar));
    REQUIRE(file.read("double").asElement<double>()() == 3.14);

    matioCpp::Element<int> intVar("int", 5);
    REQUIRE(file.write(intVar));
    REQUIRE(file.read("int").asElement<double>()() == 5); //For some reason, when storing an int into a MAT4, it is retrieved as a double

    matioCpp::Element<matioCpp::Logical> logicalVar("boolean", true);
    REQUIRE(file.write(logicalVar));
    REQUIRE(file.read("boolean").asElement<double>() == 1.0); //For some reason, when storing an bool into a MAT4, it is retrieved as a double

    matioCpp::MultiDimensionalArray<double> matrixInput("matrix", {3,3});
    for (size_t i = 0; i < 3; ++i)
    {
        for (size_t j = 0; j < 3; ++j)
        {
            matrixInput({i,j}) = i + 3.0*j;
        }
    }
    REQUIRE(file.write(matrixInput, matioCpp::Compression::zlib));

    matioCpp::MultiDimensionalArray<double> matrix = file.read("matrix").asMultiDimensionalArray<double>();
    REQUIRE(matrix.isValid());
    REQUIRE(matrix.dimensions().size() == 2);
    REQUIRE(matrix.dimensions()[0] == 3);
    REQUIRE(matrix.dimensions()[1] == 3);

    for (size_t i = 0; i < 3; ++i)
    {
        for (size_t j = 0; j < 3; ++j)
        {
            REQUIRE(matrix({i,j}) == i + 3*j);
        }
    }

    std::vector<double> data({1, 2, 3, 4, 5, 6});
    matioCpp::Vector<double> vectorInput("vector", data);
    REQUIRE(file.write(vectorInput, matioCpp::Compression::zlib));
    matioCpp::Vector<double> vector = file.read("vector").asVector<double>();
    REQUIRE(vector.isValid());
    REQUIRE(vector.size() == 6);
    for (size_t i = 0; i < 5; ++i)
    {
        REQUIRE(vector[i] == i+1);
    }

    std::vector<bool> boolData({true, false, true, false});
    matioCpp::Vector<matioCpp::Logical> boolVectorInput("bool_vector", boolData);
    REQUIRE(file.write(boolVectorInput));
    matioCpp::Vector<double> boolVector = file.read("bool_vector").asVector<double>();
    REQUIRE(boolVector.isValid());
    REQUIRE(boolVector.size() == 4);
    REQUIRE(boolVector(0));
    REQUIRE_FALSE(boolVector(1));
    REQUIRE(boolVector(2));
    REQUIRE_FALSE(boolVector(3));
}

TEST_CASE("Open not existing file")
{
    matioCpp::File file;
    REQUIRE_FALSE(file.open("notExisting.mat"));
}
