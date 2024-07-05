/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */


#include <matioCpp/StructArray.h>

bool matioCpp::StructArray::checkCompatibility(const matvar_t* inputPtr, matioCpp::VariableType variableType, matioCpp::ValueType) const
{
    if ((variableType != matioCpp::VariableType::StructArray) &&
        (variableType != matioCpp::VariableType::Struct))
    {
        std::cerr << "[matioCpp::StructArray::checkCompatibility] The variable type is not compatible with a struct array." << std::endl;
        return false;
    }

    if (inputPtr->isComplex)
    {
        std::cerr << "[matioCpp::StructArray::checkCompatibility] Cannot use a complex variable into a non-complex one." << std::endl;
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
    bool abort = false;
    matioCpp::StructArray::index_type totalElements = 1;
    for (matioCpp::StructArray::index_type dim : dimensions)
    {
        totalElements *= dim;
    }

    if (totalElements != elements.size())
    {
        std::cerr << "[ERROR][matioCpp::StructArray::StructArray] The size of elements vector does not match the provided dimensions. The total number is different." << std::endl;
        assert(false);
        abort = true;
    }


    if (!abort && elements.size() > 0)
    {
        size_t firstNumberOfFields = elements[0].numberOfFields();
        char * const * firstFields = elements[0].getStructFields();

        std::vector<matvar_t*> vectorOfPointers(totalElements * firstNumberOfFields + 1, nullptr);

        size_t innerIndex = 0;

        for (size_t i = 0; (i < totalElements) && !abort; ++i)
        {
            if (!elements[i].isValid())
            {
                std::cerr << "[ERROR][matioCpp::StructArray::StructArray] The element at index "<< i << " (0-based) is not valid." << std::endl;
                abort = true;
                assert(false);
            }

            if (elements[i].numberOfFields() != firstNumberOfFields)
            {
                std::cerr << "[ERROR][matioCpp::StructArray::StructArray] The element at index "<< i << " (0-based) has a number of fields different from the others. All Structs are supposed to have the same set of fields." << std::endl;
                abort = true;
                assert(false);
            }
            char * const * otherFields = elements[i].getStructFields();

            for (size_t field = 0; (field < firstNumberOfFields) && !abort; ++field)
            {
                if (strcmp(firstFields[field], otherFields[field]) != 0)
                {
                    std::cerr << "[ERROR][matioCpp::StructArray::StructArray] The element at index "<< i << " (0-based) has a set of fields different from the others. All Structs are supposed to have the same set of fields." << std::endl;
                    abort = true;
                    assert(false);
                }
                vectorOfPointers[innerIndex] = matioCpp::MatvarHandler::GetMatvarDuplicate(elements[i][field].toMatio());
                innerIndex++;
            }
        }
        assert(innerIndex == totalElements * firstNumberOfFields);

        if (!abort)
        {
            initializeVariable(name,
                               VariableType::StructArray,
                               matioCpp::ValueType::VARIABLE, dimensions,
                               vectorOfPointers.data());
        }
        else
        {
            initializeVariable(name,
                               VariableType::StructArray,
                               matioCpp::ValueType::VARIABLE, dimensions,
                               nullptr);
        }

    }
    else
    {
        initializeVariable(name,
                           VariableType::StructArray,
                           matioCpp::ValueType::VARIABLE, dimensions,
                           nullptr);
    }

}

matioCpp::StructArray::StructArray(const std::string &name, const std::vector<matioCpp::StructArray::index_type> &dimensions, const std::vector<std::string> &fields)
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

    for (const std::string& field : fields)
    {
        addField(field);
    }
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
    if (!handler.get() || !checkCompatibility(handler.get(), handler.variableType(), handler.valueType()))
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
    matioCpp::StructArray::index_type totalElements = 1;
    for (matioCpp::StructArray::index_type dim : dimensions)
    {
        totalElements *= dim;
    }

    if (totalElements != elements.size())
    {
        std::cerr << "[ERROR][matioCpp::StructArray::fromVectorOfStructs] The size of elements vector does not match the provided dimensions. The total number is different." << std::endl;
        return false;
    }


    if (elements.size() > 0)
    {
        size_t firstNumberOfFields = elements[0].numberOfFields();
        char * const * firstFields = elements[0].getStructFields();

        std::vector<matvar_t*> vectorOfPointers(totalElements * firstNumberOfFields + 1, nullptr);

        size_t innerIndex = 0;

        for (size_t i = 0; i < totalElements; ++i)
        {
            if (!elements[i].isValid())
            {
                std::cerr << "[ERROR][matioCpp::StructArray::fromVectorOfStructs] The element at index "<< i << " (0-based) is not valid." << std::endl;
                return false;
            }

            if (elements[i].numberOfFields() != firstNumberOfFields)
            {
                std::cerr << "[ERROR][matioCpp::StructArray::fromVectorOfStructs] The element at index "<< i << " (0-based) has a number of fields different from the others. All Structs are supposed to have the same set of fields." << std::endl;
                return false;
            }
            char * const * otherFields = elements[i].getStructFields();

            for (size_t field = 0; field < firstNumberOfFields; ++field)
            {
                if (strcmp(firstFields[field], otherFields[field]) != 0)
                {
                    std::cerr << "[ERROR][matioCpp::StructArray::fromVectorOfStructs] The element at index "<< i << " (0-based) has a set of fields different from the others. All Structs are supposed to have the same set of fields." << std::endl;
                    return false;
                }
                vectorOfPointers[innerIndex] = matioCpp::MatvarHandler::GetMatvarDuplicate(elements[i][field].toMatio());
                innerIndex++;
            }
        }
        assert(innerIndex == totalElements * firstNumberOfFields);


        return initializeVariable(name(),
                                  VariableType::StructArray,
                                  matioCpp::ValueType::VARIABLE, dimensions,
                                  vectorOfPointers.data());
    }
    else
    {

        return initializeVariable(name(),
                                  VariableType::StructArray,
                                  matioCpp::ValueType::VARIABLE, dimensions,
                                  nullptr);
    }
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
    return changeName(newName);
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
        for (size_t i = 0; i < numberOfFields; ++i)
        {
            output.emplace_back(matvarOutput[i]);
        }
    }

    return output;
}

void matioCpp::StructArray::clear()
{
    fromOther(std::move(StructArray(name())));
}

bool matioCpp::StructArray::isFieldExisting(const std::string &field) const
{
    size_t index;
    return getStructFieldIndex(field, index);
}

size_t matioCpp::StructArray::getFieldIndex(const std::string &field) const
{
    size_t index;
    getStructFieldIndex(field, index);
    return index;
}

bool matioCpp::StructArray::addField(const std::string &newField)
{
    if (!addStructField(newField))
    {
        std::cerr << "[ERROR][matioCpp::StructArray::addField] Failed to add field " << newField << "." <<std::endl;
    }
    return true;
}

bool matioCpp::StructArray::addFields(const std::vector<std::string> &newFields)
{
    for (const std::string& field : newFields)
    {
        if (!addField(field))
        {
            return false;
        }
    }
    return true;
}

bool matioCpp::StructArray::setElement(const std::vector<matioCpp::StructArray::index_type> &el, const matioCpp::Struct &newValue)
{
    return setElement(rawIndexFromIndices(el), newValue);
}

bool matioCpp::StructArray::setElement(matioCpp::StructArray::index_type el, const matioCpp::Struct &newValue)
{
    assert(el < numberOfElements() && "The requested element is out of bounds.");

    char * const * arrayFields = getStructFields();
    char * const * structFields = newValue.getStructFields();

    if (numberOfFields() != newValue.numberOfFields())
    {
        std::cerr << "[ERROR][matioCpp::StructArray::setElement] The input struct is supposed to have the same number of fields of the struct array." <<std::endl;
        return false;
    }

    for (size_t i = 0; i < numberOfFields(); ++i)
    {
        if (strcmp(arrayFields[i], structFields[i]) != 0)
        {
            std::cerr << "[ERROR][matioCpp::StructArray::setElement] The field " << structFields[i] << " of the input struct is supposed to be " << arrayFields[i]
                      << ". Cannot insert in a struct array a new field in a single element." <<std::endl;
            return false;
        }

        bool ok = setStructField(i, newValue(i), el);
        if (!ok)
        {
            std::cerr << "[ERROR][matioCpp::StructArray::setElement] Failed to set field " << structFields[i] << "." <<std::endl;
            return false;
        }
    }

    return true;
}

matioCpp::StructArray::Element matioCpp::StructArray::operator ()(const std::vector<matioCpp::StructArray::index_type> &el)
{
    matioCpp::StructArray::index_type linearindex = rawIndexFromIndices(el);
    return matioCpp::StructArray::Element(linearindex, this);
}

matioCpp::StructArray::ConstElement matioCpp::StructArray::operator ()(const std::vector<matioCpp::StructArray::index_type> &el) const
{
    matioCpp::StructArray::index_type linearindex = rawIndexFromIndices(el);
    return matioCpp::StructArray::ConstElement(linearindex, this);
}

matioCpp::StructArray::Element matioCpp::StructArray::operator ()(matioCpp::StructArray::index_type el)
{
    assert(el < numberOfElements());
    return matioCpp::StructArray::Element(el, this);
}

matioCpp::StructArray::ConstElement matioCpp::StructArray::operator ()(matioCpp::StructArray::index_type el) const
{
    assert(el < numberOfElements());
    return matioCpp::StructArray::ConstElement(el, this);
}

matioCpp::StructArray::Element matioCpp::StructArray::operator [](const std::vector<matioCpp::StructArray::index_type> &el)
{
    matioCpp::StructArray::index_type linearindex = rawIndexFromIndices(el);
    return matioCpp::StructArray::Element(linearindex, this);
}

matioCpp::StructArray::ConstElement matioCpp::StructArray::operator [](const std::vector<matioCpp::StructArray::index_type> &el) const
{
    matioCpp::StructArray::index_type linearindex = rawIndexFromIndices(el);
    return matioCpp::StructArray::ConstElement(linearindex, this);
}

matioCpp::StructArray::Element matioCpp::StructArray::operator [](matioCpp::StructArray::index_type el)
{
    assert(el < numberOfElements());
    return matioCpp::StructArray::Element(el, this);
}

matioCpp::StructArray::ConstElement matioCpp::StructArray::operator [](matioCpp::StructArray::index_type el) const
{
    assert(el < numberOfElements());
    return matioCpp::StructArray::ConstElement(el, this);
}


