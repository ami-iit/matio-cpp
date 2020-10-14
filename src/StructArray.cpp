/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */


#include <matioCpp/StructArray.h>

bool matioCpp::StructArray::checkCompatibility(const matvar_t *inputPtr) const
{
    if (!inputPtr)
    {
        std::cerr << "[matioCpp::StructArray::checkCompatibility] The input pointer is null." << std::endl;
        return false;
    }

    matioCpp::VariableType outputVariableType = matioCpp::VariableType::Unsupported;
    matioCpp::ValueType outputValueType = matioCpp::ValueType::UNSUPPORTED;
    get_types_from_matvart(inputPtr, outputVariableType, outputValueType);

    if (outputVariableType != matioCpp::VariableType::StructArray)
    {
        std::cerr << "[matioCpp::StructArray::checkCompatibility] The input variable is not a struct array." << std::endl;
        return false;
    }

    if (inputPtr->isComplex)
    {
        std::cerr << "[matioCpp::StructArray::checkCompatibility] Cannot copy a complex variable to a non-complex one." << std::endl;
        return false;
    }

    return true;
}

matioCpp::StructArray::StructArray()
{
    size_t emptyDimensions[] = {0, 0};
    initializeVariable("unnamed_struct_array",
                       VariableType::StructArray,
                       matioCpp::ValueType::VARIABLE, emptyDimensions,
                       nullptr);
}

matioCpp::StructArray::StructArray(const std::string &name)
{
    size_t emptyDimensions[] = {0, 0};
    initializeVariable(name,
                       VariableType::StructArray,
                       matioCpp::ValueType::VARIABLE, emptyDimensions,
                       nullptr);
}

matioCpp::StructArray::StructArray(const std::string &name, const std::vector<matioCpp::StructArray::index_type> &dimensions)
{
    for (matioCpp::StructArray::index_type dim : dimensions)
    {
        if (dim == 0)
        {
            std::cerr << "[ERROR][matioCpp::StructArray::StructArray] Zero dimension detected." << std::endl;
            assert(false);
        }
    }

    initializeVariable(name,
                       VariableType::StructArray,
                       matioCpp::ValueType::VARIABLE, dimensions,
                       nullptr);
}

matioCpp::StructArray::StructArray(const std::string &name, const std::vector<matioCpp::StructArray::index_type> &dimensions, const std::vector<matioCpp::Struct> &elements)
{
//    matioCpp::CellArray::index_type totalElements = 1;
//    for (matioCpp::CellArray::index_type dim : dimensions)
//    {
//        totalElements *= dim;
//    }

//    if (totalElements != elements.size())
//    {
//        std::cerr << "[ERROR][matioCpp::CellArray::CellArray] The size of elements vector does not match the provided dimensions. The total number is different." << std::endl;
//        assert(false);
//    }
//    std::vector<matvar_t*> vectorOfPointers(totalElements, nullptr);
//    for (size_t i = 0; i < totalElements; ++i)
//    {
//        if (!elements[i].isValid())
//        {
//            std::cerr << "[ERROR][matioCpp::CellArray::fromVectorOfVariables] The element at index "<< i << " (0-based) is not valid." << std::endl;
//            assert(false);
//        }
//        vectorOfPointers[i] = matioCpp::MatvarHandler::GetMatvarDuplicate(elements[i].toMatio());
//    }

//    initializeVariable(name,
//                       VariableType::CellArray,
//                       matioCpp::ValueType::VARIABLE, dimensions,
//                       vectorOfPointers.data());
}

matioCpp::StructArray::StructArray(const matioCpp::StructArray &other)
{
    fromOther(other);
}

matioCpp::StructArray::StructArray(matioCpp::StructArray &&other)
{
    fromOther(std::forward<StructArray>(other));
}

matioCpp::StructArray::StructArray(const MatvarHandler &handler)
    : matioCpp::Variable(handler)
{
    if (!checkCompatibility(handler.get()))
    {
        assert(false);
        size_t emptyDimensions[] = {0, 0};
        initializeVariable("unnamed_struct_array",
                           VariableType::StructArray,
                           matioCpp::ValueType::VARIABLE, emptyDimensions,
                           nullptr);
    }
}

matioCpp::StructArray::~StructArray()
{

}

matioCpp::StructArray &matioCpp::StructArray::operator=(const matioCpp::StructArray &other)
{
    fromOther(other);
    return *this;
}

matioCpp::StructArray &matioCpp::StructArray::operator=(matioCpp::StructArray &&other)
{
    fromOther(std::forward<StructArray>(other));
    return *this;
}

bool matioCpp::StructArray::fromVectorOfStructs(const std::vector<matioCpp::StructArray::index_type> &dimensions, const std::vector<matioCpp::Struct> &elements)
{
//    matioCpp::CellArray::index_type totalElements = 1;
//    for (matioCpp::CellArray::index_type dim : dimensions)
//    {
//        totalElements *= dim;
//    }

//    if (totalElements != elements.size())
//    {
//        std::cerr << "[ERROR][matioCpp::CellArray::fromVectorOfVariables] The size of elements vector does not match the provided dimensions. The total number is different." << std::endl;
//        return false;
//    }
//    std::vector<matvar_t*> vectorOfPointers(totalElements, nullptr);
//    for (size_t i = 0; i < totalElements; ++i)
//    {
//        if (!elements[i].isValid())
//        {
//            std::cerr << "[ERROR][matioCpp::CellArray::fromVectorOfVariables] The element at index "<< i << " (0-based) is not valid." << std::endl;
//            return false;
//        }
//        vectorOfPointers[i] = matioCpp::MatvarHandler::GetMatvarDuplicate(elements[i].toMatio());
//    }

//    return initializeVariable(name(),
//                              VariableType::CellArray,
//                              matioCpp::ValueType::VARIABLE, dimensions,
//                              vectorOfPointers.data());
}

matioCpp::StructArray::index_type matioCpp::StructArray::rawIndexFromIndices(const std::vector<matioCpp::StructArray::index_type> &el) const
{
    assert(dimensions().size() > 0 && numberOfElements() > 0 && "[matioCpp::StructArray::rawIndexFromIndices] The array is empty.");
    assert(el.size() > 0 == dimensions().size() > 0 && "[matioCpp::StructArray::rawIndexFromIndices] The input vector el should have the same number of dimensions of the array.");
    assert(el[0] < dimensions()[0] && "[matioCpp::StructArray::rawIndexFromIndices] The required element is out of bounds.");

    typename matioCpp::StructArray::index_type index = 0;
    typename matioCpp::StructArray::index_type previousDimensionsFactorial = 1;

    for (size_t i = 0; i < el.size(); ++i)
    {
        assert(el[i] < dimensions()[i] && "[matioCpp::StructArray::rawIndexFromIndices] The required element is out of bounds.");
        index += el[i] * previousDimensionsFactorial;
        previousDimensionsFactorial *= dimensions()[i];
    }

    return index;
}

bool matioCpp::StructArray::indicesFromRawIndex(size_t rawIndex, std::vector<matioCpp::StructArray::index_type> &el) const
{
    el.resize(dimensions().size());

    if (rawIndex >= numberOfElements())
    {
        std::cerr << "[ERROR][matioCpp::StructArray::indicesFromRawIndex] rawIndex is greater than the number of elements." << std::endl;
        return false;
    }

    size_t previousDimensionsFactorial = dimensions()[0];

    //First we fill el with the factorial of the dimensions

    for (size_t i = 1; i < el.size(); ++i)
    {
        el[i - 1] = previousDimensionsFactorial;
        previousDimensionsFactorial *= dimensions()[i];
    }

    size_t remainder = rawIndex;

    for (size_t i = el.size() - 1; i > 0; --i)
    {
        el[i] = remainder / el[i - 1];
        remainder -= el[i] * el[i - 1];
    }
    el[0] = remainder;

    return true;
}

bool matioCpp::StructArray::setName(const std::string &newName)
{
    return initializeVariable(newName,
                              VariableType::StructArray,
                              matioCpp::ValueType::VARIABLE, dimensions(),
                              (void*)toMatio()->data);
}

void matioCpp::StructArray::resize(const std::vector<matioCpp::StructArray::index_type> &newDimensions)
{
    matioCpp::StructArray newArray(name(), newDimensions);
    fromOther(std::move(newArray));
}

matioCpp::StructArray::index_type matioCpp::StructArray::numberOfElements() const
{
    return getArrayNumberOfElements();
}

matioCpp::StructArray::index_type matioCpp::StructArray::numberOfFields() const
{
    return getStructNumberOfFields();
}

std::vector<std::string> matioCpp::StructArray::fields() const
{
    std::vector<std::string> output;
    char * const * matvarOutput = getStructFields();
    if (matvarOutput)
    {
        size_t numberOfFields = getStructNumberOfFields();
        output.reserve(numberOfFields);
        for (size_t i = 0; i < output.size(); ++i)
        {
            output.emplace_back(matvarOutput[i]);
        }
    }

    return output;
}

bool matioCpp::StructArray::isFieldExisting(const std::string &field) const
{
    return getStructFieldIndex(field) < numberOfFields();
}

size_t matioCpp::StructArray::getFieldIndex(const std::string &field) const
{
    return getStructFieldIndex(field);
}

bool matioCpp::StructArray::addField(const std::string &newField)
{
    return addStructField(newField);
}

bool matioCpp::StructArray::setElement(const std::vector<matioCpp::StructArray::index_type> &el, const matioCpp::Struct &newValue)
{

}

bool matioCpp::StructArray::setElement(matioCpp::StructArray::index_type el, const matioCpp::Struct &newValue)
{

}

matioCpp::Struct matioCpp::StructArray::operator()(const std::vector<matioCpp::StructArray::index_type> &el)
{

}

const matioCpp::Struct matioCpp::StructArray::operator()(const std::vector<matioCpp::StructArray::index_type> &el) const
{

}

matioCpp::Struct matioCpp::StructArray::operator()(matioCpp::StructArray::index_type el)
{

}

const matioCpp::Struct matioCpp::StructArray::operator()(matioCpp::StructArray::index_type el) const
{

}

matioCpp::Struct matioCpp::StructArray::operator[](const std::vector<matioCpp::StructArray::index_type> &el)
{

}

const matioCpp::Struct matioCpp::StructArray::operator[](const std::vector<matioCpp::StructArray::index_type> &el) const
{

}

matioCpp::Struct matioCpp::StructArray::operator[](matioCpp::StructArray::index_type el)
{

}

const matioCpp::Struct matioCpp::StructArray::operator[](matioCpp::StructArray::index_type el) const
{

}
