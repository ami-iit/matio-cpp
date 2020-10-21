/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

#include <matioCpp/CellArray.h>

bool matioCpp::CellArray::checkCompatibility(const matvar_t *inputPtr) const
{
    if (!inputPtr)
    {
        std::cerr << "[matioCpp::CellArray::checkCompatibility] The input pointer is null." << std::endl;
        return false;
    }

    matioCpp::VariableType outputVariableType = matioCpp::VariableType::Unsupported;
    matioCpp::ValueType outputValueType = matioCpp::ValueType::UNSUPPORTED;
    get_types_from_matvart(inputPtr, outputVariableType, outputValueType);

    if (outputVariableType != matioCpp::VariableType::CellArray)
    {
        std::cerr << "[matioCpp::CellArray::checkCompatibility] The input variable is not a cell array." << std::endl;
        return false;
    }

    if (inputPtr->isComplex)
    {
        std::cerr << "[matioCpp::CellArray::checkCompatibility] Cannot copy a complex variable to a non-complex one." << std::endl;
        return false;
    }

    return true;
}

matioCpp::CellArray::CellArray()
{
    size_t emptyDimensions[] = {0, 0};
    initializeVariable("unnamed_cell_array",
                       VariableType::CellArray,
                       matioCpp::ValueType::VARIABLE, emptyDimensions,
                       nullptr);
}

matioCpp::CellArray::CellArray(const std::string &name)
{
    size_t emptyDimensions[] = {0, 0};
    initializeVariable(name,
                       VariableType::CellArray,
                       matioCpp::ValueType::VARIABLE, emptyDimensions,
                       nullptr);
}

matioCpp::CellArray::CellArray(const std::string &name, const std::vector<matioCpp::CellArray::index_type> &dimensions)
{
    for (matioCpp::CellArray::index_type dim : dimensions)
    {
        if (dim == 0)
        {
            std::cerr << "[ERROR][matioCpp::CellArray::CellArray] Zero dimension detected." << std::endl;
            assert(false);
        }
    }

    initializeVariable(name,
                       VariableType::CellArray,
                       matioCpp::ValueType::VARIABLE, dimensions,
                       nullptr);
}

matioCpp::CellArray::CellArray(const std::string &name, const std::vector<matioCpp::CellArray::index_type> &dimensions, std::vector<matioCpp::Variable> &elements)
{
    matioCpp::CellArray::index_type totalElements = 1;
    for (matioCpp::CellArray::index_type dim : dimensions)
    {
        totalElements *= dim;
    }

    if (totalElements != elements.size())
    {
        std::cerr << "[ERROR][matioCpp::CellArray::CellArray] The size of elements vector does not match the provided dimensions. The total number is different." << std::endl;
        assert(false);
    }
    std::vector<matvar_t*> vectorOfPointers(totalElements, nullptr);
    for (size_t i = 0; i < totalElements; ++i)
    {
        if (!elements[i].isValid())
        {
            std::cerr << "[ERROR][matioCpp::CellArray::CellArray] The element at index "<< i << " (0-based) is not valid." << std::endl;
            assert(false);
        }
        vectorOfPointers[i] = matioCpp::MatvarHandler::GetMatvarDuplicate(elements[i].toMatio());
    }

    initializeVariable(name,
                       VariableType::CellArray,
                       matioCpp::ValueType::VARIABLE, dimensions,
                       vectorOfPointers.data());
}

matioCpp::CellArray::CellArray(const CellArray &other)
{
    fromOther(other);
}

matioCpp::CellArray::CellArray(CellArray &&other)
{
    fromOther(std::forward<CellArray>(other));
}

matioCpp::CellArray::CellArray(const MatvarHandler &handler)
    : matioCpp::Variable(handler)
{
    if (!checkCompatibility(handler.get()))
    {
        assert(false);
        size_t emptyDimensions[] = {0, 0};
        initializeVariable("unnamed_cell_array",
                           VariableType::CellArray,
                           matioCpp::ValueType::VARIABLE, emptyDimensions,
                           nullptr);
    }
}

matioCpp::CellArray::~CellArray()
{

}

matioCpp::CellArray &matioCpp::CellArray::operator=(const matioCpp::CellArray &other)
{
    fromOther(other);
    return *this;
}

matioCpp::CellArray &matioCpp::CellArray::operator=(matioCpp::CellArray &&other)
{
    fromOther(std::forward<CellArray>(other));
    return *this;
}

bool matioCpp::CellArray::fromVectorOfVariables(const std::vector<matioCpp::CellArray::index_type> &dimensions, std::vector<matioCpp::Variable> &elements)
{
    matioCpp::CellArray::index_type totalElements = 1;
    for (matioCpp::CellArray::index_type dim : dimensions)
    {
        totalElements *= dim;
    }

    if (totalElements != elements.size())
    {
        std::cerr << "[ERROR][matioCpp::CellArray::fromVectorOfVariables] The size of elements vector does not match the provided dimensions. The total number is different." << std::endl;
        return false;
    }
    std::vector<matvar_t*> vectorOfPointers(totalElements, nullptr);
    for (size_t i = 0; i < totalElements; ++i)
    {
        if (!elements[i].isValid())
        {
            std::cerr << "[ERROR][matioCpp::CellArray::fromVectorOfVariables] The element at index "<< i << " (0-based) is not valid." << std::endl;
            return false;
        }
        vectorOfPointers[i] = matioCpp::MatvarHandler::GetMatvarDuplicate(elements[i].toMatio());
    }

    return initializeVariable(name(),
                              VariableType::CellArray,
                              matioCpp::ValueType::VARIABLE, dimensions,
                              vectorOfPointers.data());
}

typename matioCpp::CellArray::index_type matioCpp::CellArray::rawIndexFromIndices(const std::vector<typename matioCpp::CellArray::index_type> &el) const
{
    assert(dimensions().size() > 0 && numberOfElements() > 0 && "[matioCpp::CellArray::rawIndexFromIndices] The array is empty.");
    assert(el.size() > 0 == dimensions().size() > 0 && "[matioCpp::CellArray::rawIndexFromIndices] The input vector el should have the same number of dimensions of the array.");
    assert(el[0] < dimensions()[0] && "[matioCpp::CellArray::rawIndexFromIndices] The required element is out of bounds.");

    typename matioCpp::CellArray::index_type index = 0;
    typename matioCpp::CellArray::index_type previousDimensionsFactorial = 1;

    for (size_t i = 0; i < el.size(); ++i)
    {
        assert(el[i] < dimensions()[i] && "[matioCpp::CellArray::rawIndexFromIndices] The required element is out of bounds.");
        index += el[i] * previousDimensionsFactorial;
        previousDimensionsFactorial *= dimensions()[i];
    }

    return index;
}

bool matioCpp::CellArray::indicesFromRawIndex(size_t rawIndex, std::vector<matioCpp::CellArray::index_type> &el) const
{
    el.resize(dimensions().size());

    if (rawIndex >= numberOfElements())
    {
        std::cerr << "[ERROR][matioCpp::CellArray::indicesFromRawIndex] rawIndex is greater than the number of elements." << std::endl;
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

bool matioCpp::CellArray::setName(const std::string &newName)
{
    return initializeVariable(newName,
                              VariableType::CellArray,
                              matioCpp::ValueType::VARIABLE, dimensions(),
                              (void*)toMatio()->data);
}

void matioCpp::CellArray::resize(const std::vector<typename matioCpp::CellArray::index_type> &newDimensions)
{
    matioCpp::CellArray newArray(name(), newDimensions);
    fromOther(std::move(newArray));
}

typename matioCpp::CellArray::index_type matioCpp::CellArray::numberOfElements() const
{
    matioCpp::CellArray::index_type totalElements = 1;
    for (matioCpp::CellArray::index_type dim : dimensions())
    {
        totalElements *= dim;
    }

    return totalElements;
}

bool matioCpp::CellArray::setElement(const std::vector<matioCpp::CellArray::index_type> &el, const matioCpp::Variable &newValue)
{
    if (!isValid())
    {
        std::cerr << "[ERROR][matioCpp::CellArray::setElement] The CellArray has not been properly initialized." << std::endl;
        return false;
    }

    if (!newValue.isValid())
    {
        std::cerr << "[ERROR][matioCpp::CellArray::setElement] The input variable is not valid." << std::endl;
        return false;
    }

    if (!setCellElement(rawIndexFromIndices(el), newValue))
    {
        std::cerr << "[ERROR][matioCpp::CellArray::setElement] Failed to set the cell element." << std::endl;
        return false;
    }

    return true;
}

bool matioCpp::CellArray::setElement(matioCpp::CellArray::index_type el, const matioCpp::Variable &newValue)
{
    assert(el < numberOfElements() && "[matioCpp::CellArray::setElement] The required element is out of bounds.");

    if (!isValid())
    {
        std::cerr << "[ERROR][matioCpp::CellArray::setElement] The CellArray has not been properly initialized." << std::endl;
        return false;
    }

    if (!newValue.isValid())
    {
        std::cerr << "[ERROR][matioCpp::CellArray::setElement] The input variable is not valid." << std::endl;
        return false;
    }

    if (!setCellElement(el, newValue))
    {
        std::cerr << "[ERROR][matioCpp::CellArray::setElement] Failed to set the cell element." << std::endl;
        return false;
    }

    return true;
}

matioCpp::Variable matioCpp::CellArray::operator()(const std::vector<matioCpp::CellArray::index_type> &el)
{
    return getCellElement(rawIndexFromIndices(el));
}

const matioCpp::Variable matioCpp::CellArray::operator()(const std::vector<matioCpp::CellArray::index_type> &el) const
{
    return getCellElement(rawIndexFromIndices(el));
}

matioCpp::Variable matioCpp::CellArray::operator()(matioCpp::CellArray::index_type el)
{
    assert(el < numberOfElements() && "[matioCpp::CellArray::operator()] The required element is out of bounds.");
    return getCellElement(el);
}

const matioCpp::Variable matioCpp::CellArray::operator()(matioCpp::CellArray::index_type el) const
{
    assert(el < numberOfElements() && "[matioCpp::CellArray::operator()] The required element is out of bounds.");
    return getCellElement(el);
}

matioCpp::Variable matioCpp::CellArray::operator[](const std::vector<matioCpp::CellArray::index_type> &el)
{
    return getCellElement(rawIndexFromIndices(el));
}

const matioCpp::Variable matioCpp::CellArray::operator[](const std::vector<matioCpp::CellArray::index_type> &el) const
{
    return getCellElement(rawIndexFromIndices(el));
}

matioCpp::Variable matioCpp::CellArray::operator[](matioCpp::CellArray::index_type el)
{
    assert(el < numberOfElements() && "[matioCpp::CellArray::operator[]] The required element is out of bounds.");
    return getCellElement(el);
}

const matioCpp::Variable matioCpp::CellArray::operator[](matioCpp::CellArray::index_type el) const
{
    assert(el < numberOfElements() && "[matioCpp::CellArray::operator[]] The required element is out of bounds.");
    return getCellElement(el);
}

