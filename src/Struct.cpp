/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */


#include <matioCpp/Struct.h>

bool matioCpp::Struct::checkCompatibility(const matvar_t* inputPtr, matioCpp::VariableType variableType, matioCpp::ValueType) const
{

    if (variableType != matioCpp::VariableType::Struct)
    {
        std::cerr << "[matioCpp::Struct::checkCompatibility] The variable type is not compatible with a struct." << std::endl;
        return false;
    }

    if (inputPtr->isComplex)
    {
        std::cerr << "[matioCpp::Struct::checkCompatibility] Cannot use a complex variable into a non-complex one." << std::endl;
        return false;
    }

    return true;
}

matioCpp::Struct::Struct()
{
    size_t emptyDimensions[] = {1, 1};
    initializeVariable("unnamed_struct",
                       VariableType::Struct,
                       matioCpp::ValueType::VARIABLE, emptyDimensions,
                       nullptr);
}

matioCpp::Struct::Struct(const std::string &name)
{
    size_t emptyDimensions[] = {1, 1};
    initializeVariable(name,
                       VariableType::Struct,
                       matioCpp::ValueType::VARIABLE, emptyDimensions,
                       nullptr);
}

matioCpp::Struct::Struct(const std::string &name, const std::vector<Variable> &elements)
{
    size_t emptyDimensions[] = {1, 1};
    std::vector<matvar_t*> vectorOfPointers;
    for (size_t i = 0; i < elements.size(); ++i)
    {
        if (elements[i].isValid())
        {
            vectorOfPointers.push_back(matioCpp::MatvarHandler::GetMatvarDuplicate(elements[i].toMatio()));
        }
        else
        {
            std::cerr << "[ERROR][matioCpp::Struct::Struct] The element of " << name << " at index " << i << " (0-based) is not valid. It will be skipped." << std::endl;
        }
    }
    vectorOfPointers.push_back(nullptr);  //The vector of pointers has to be null terminated

    initializeVariable(name,
                       VariableType::Struct,
                       matioCpp::ValueType::VARIABLE, emptyDimensions,
                       vectorOfPointers.data());
}

matioCpp::Struct::Struct(const matioCpp::Struct &other)
{
    fromOther(other);
}

matioCpp::Struct::Struct(matioCpp::Struct &&other)
{
    fromOther(std::forward<Struct>(other));
}

matioCpp::Struct::Struct(const MatvarHandler &handler)
    : matioCpp::Variable(handler)
{
    if (!handler.get() || !checkCompatibility(handler.get(), handler.variableType(), handler.valueType()))
    {
        assert(false);
        size_t emptyDimensions[] = {1, 1};
        initializeVariable("unnamed_struct",
                           VariableType::Struct,
                           matioCpp::ValueType::VARIABLE, emptyDimensions,
                           nullptr);
    }
}

matioCpp::Struct::~Struct()
{

}

matioCpp::Struct &matioCpp::Struct::operator=(const matioCpp::Struct &other)
{
    fromOther(other);
    return *this;
}

matioCpp::Struct &matioCpp::Struct::operator=(matioCpp::Struct &&other)
{
    fromOther(std::forward<Struct>(other));
    return *this;
}

bool matioCpp::Struct::fromVectorOfVariables(const std::vector<Variable> &elements)
{
    std::vector<matvar_t*> vectorOfPointers(elements.size() + 1, nullptr); //The vector of pointers has to be null terminated
    for (size_t i = 0; i < elements.size(); ++i)
    {
        if (!elements[i].isValid())
        {
            std::cerr << "[ERROR][matioCpp::Struct::fromVectorOfVariables] The element at index "<< i << " (0-based) is not valid." << std::endl;
            return false;
        }
        vectorOfPointers[i] = matioCpp::MatvarHandler::GetMatvarDuplicate(elements[i].toMatio());
    }

    initializeVariable(name(),
                       VariableType::Struct,
                       matioCpp::ValueType::VARIABLE, dimensions(),
                       vectorOfPointers.data());
    return true;
}

bool matioCpp::Struct::setName(const std::string &newName)
{
    return changeName(newName);
}

matioCpp::Struct::index_type matioCpp::Struct::numberOfFields() const
{
    return getStructNumberOfFields();
}

std::vector<std::string> matioCpp::Struct::fields() const
{
    std::vector<std::string> output;
    char * const * matvarOutput = getStructFields();
    if (matvarOutput)
    {
        size_t numberOfFields = getStructNumberOfFields();
        output.reserve(numberOfFields);
        for (size_t i = 0; i < numberOfFields; ++i)
        {
            output.emplace_back(matvarOutput[i]);
        }
    }

    return output;
}

void matioCpp::Struct::clear()
{
    fromOther(std::move(Struct(name())));
}

bool matioCpp::Struct::isFieldExisting(const std::string &field) const
{
    size_t index;
    return getStructFieldIndex(field, index);
}

size_t matioCpp::Struct::getFieldIndex(const std::string &field) const
{
    size_t index;
    getStructFieldIndex(field, index);
    return index;
}

bool matioCpp::Struct::setField(matioCpp::Struct::index_type index, const matioCpp::Variable &newValue)
{
    assert(index < numberOfFields() && "The specified index is out of bounds");
    return setStructField(index, newValue);
}

bool matioCpp::Struct::setField(const std::string &field, const matioCpp::Variable &newValue)
{
    return setStructField(field, newValue);
}

bool matioCpp::Struct::setField(const matioCpp::Variable &newValue)
{
    return setStructField(newValue.name(), newValue);
}

matioCpp::Variable matioCpp::Struct::operator()(matioCpp::Struct::index_type el)
{
    assert(el < numberOfFields() && "The specified index is out of bounds");
    return getStructField(el);
}

const matioCpp::Variable matioCpp::Struct::operator()(matioCpp::Struct::index_type el) const
{
    assert(el < numberOfFields() && "The specified index is out of bounds");
    return getStructField(el);
}

matioCpp::Variable matioCpp::Struct::operator()(const std::string &el)
{
    size_t index = getFieldIndex(el);
    assert(index < numberOfFields() && "The specified field does not exist.");
    return getStructField(index);
}

const matioCpp::Variable matioCpp::Struct::operator()(const std::string &el) const
{
    size_t index = getFieldIndex(el);
    assert(index < numberOfFields() && "The specified field does not exist.");
    return getStructField(index);
}

matioCpp::Variable matioCpp::Struct::operator[](matioCpp::Struct::index_type el)
{
    assert(el < numberOfFields() && "The specified index is out of bounds");
    return getStructField(el);
}

const matioCpp::Variable matioCpp::Struct::operator[](matioCpp::Struct::index_type el) const
{
    assert(el < numberOfFields() && "The specified index is out of bounds");
    return getStructField(el);
}
