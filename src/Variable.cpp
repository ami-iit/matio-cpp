/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */


#include <matioCpp/Variable.h>
#include <matioCpp/CellArray.h>
#include <matioCpp/Struct.h>
#include <matioCpp/StructArray.h>

bool matioCpp::Variable::initializeVariable(const std::string& name, const VariableType& variableType, const ValueType& valueType, matioCpp::Span<const size_t> dimensions, void* data)
{
    std::string errorPrefix = "[ERROR][matioCpp::Variable::createVar] ";
    if (name.empty())
    {
        std::cerr << errorPrefix << "The name should not be empty." << std::endl;
        return false;
    }

    if (dimensions.size() < 2)
    {
        std::cerr << errorPrefix << "The dimensions should be at least 2." << std::endl;
        return false;
    }

    matio_types matioType;
    matio_classes matioClass;

    if(!get_matio_types(variableType, valueType, matioClass, matioType))
    {
        std::cerr << errorPrefix << "Either the variableType or the valueType are not supported." << std::endl;
        return false;
    }

    std::vector<size_t> dimensionsCopy;
    dimensionsCopy.assign(dimensions.begin(), dimensions.end()); //This is needed since Mat_VarCreate needs a non-const pointer for the dimensions. This method already allocates memory

    matvar_t* newPtr = Mat_VarCreate(name.c_str(), matioClass, matioType, dimensionsCopy.size(), dimensionsCopy.data(), data, 0);

    if (m_handler)
    {
        if (!m_handler->importMatvar(newPtr))
        {
            std::cerr << errorPrefix << "Failed to modify the variable." << std::endl;
            MatvarHandler::DeleteMatvar(newPtr);
            return false;
        }
    }
    else
    {
        m_handler = new matioCpp::SharedMatvar(newPtr);
    }

    if (!m_handler || !m_handler->get())
    {
        std::cerr << errorPrefix << "Failed to create the variable." << std::endl;
        return false;
    }

    return true;
}

bool matioCpp::Variable::initializeComplexVariable(const std::string& name, const VariableType& variableType, const ValueType& valueType, matioCpp::Span<const size_t> dimensions, void *realData, void *imaginaryData)
{
    std::string errorPrefix = "[ERROR][matioCpp::Variable::createComplexVar] ";
    if (name.empty())
    {
        std::cerr << errorPrefix << "The name should not be empty." << std::endl;
        return false;
    }

    if (dimensions.size() < 2)
    {
        std::cerr << errorPrefix << "The dimensions should be at least 2." << std::endl;
        return false;
    }

    if (!realData)
    {
        std::cerr << errorPrefix << "The real data pointer is empty." << std::endl;
        return false;
    }

    if (!imaginaryData)
    {
        std::cerr << errorPrefix << "The imaginary data pointer is empty." << std::endl;
        return false;
    }

    matio_types matioType;
    matio_classes matioClass;

    if (!get_matio_types(variableType, valueType, matioClass, matioType))
    {
        std::cerr << errorPrefix << "Either the variableType or the valueType are not supported." << std::endl;
        return false;
    }

    mat_complex_split_t matioComplexSplit;
    matioComplexSplit.Re = realData;
    matioComplexSplit.Im = imaginaryData;

    std::vector<size_t> dimensionsCopy;
    dimensionsCopy.assign(dimensions.begin(), dimensions.end()); //This is needed since Mat_VarCreate needs a non-const pointer for the dimensions. This method already allocates memory

    matvar_t* newPtr = Mat_VarCreate(name.c_str(), matioClass, matioType, dimensionsCopy.size(), dimensionsCopy.data(), &matioComplexSplit, MAT_F_COMPLEX); //Data is hard copied, since the flag MAT_F_DONT_COPY_DATA is not used

    if (m_handler)
    {
        if (!m_handler->importMatvar(newPtr))
        {
            std::cerr << errorPrefix << "Failed to modify the variable." << std::endl;
            MatvarHandler::DeleteMatvar(newPtr);
            return false;
        }
    }
    else
    {
        m_handler = new matioCpp::SharedMatvar(newPtr);
    }

    if (!m_handler || !m_handler->get())
    {
        std::cerr << errorPrefix << "Failed to create the variable." << std::endl;
        return false;
    }

    return true;
}

bool matioCpp::Variable::changeName(const std::string &newName)
{
    if (!isValid())
    {
        return false;
    }

    char* previousName = m_handler->get()->name;

    if (previousName)
    {
        free(previousName);
    }

    m_handler->get()->name = strdup(newName.c_str());

    return (name() == newName);
}

size_t matioCpp::Variable::getArrayNumberOfElements() const
{
    size_t totalElements = 1;
    for (size_t dim : dimensions())
    {
        totalElements *= dim;
    }

    return totalElements;
}

bool matioCpp::Variable::setCellElement(size_t linearIndex, const matioCpp::Variable &newValue)
{
    Variable copiedNonOwning(matioCpp::WeakMatvar(matioCpp::MatvarHandler::GetMatvarDuplicate(newValue.toMatio()), m_handler));
    if (!copiedNonOwning.isValid())
    {
        return false;
    }

    matvar_t* previousCell = Mat_VarSetCell(m_handler->get(), linearIndex, copiedNonOwning.toMatio());

    m_handler->dropOwnedPointer(previousCell); //This avoids that any variable that was using this pointer before tries to access it.
    MatvarHandler::DeleteMatvar(previousCell);

    return Mat_VarGetCell(m_handler->get(), linearIndex);
}

matioCpp::Variable matioCpp::Variable::getCellElement(size_t linearIndex)
{
    return Variable(matioCpp::WeakMatvar(Mat_VarGetCell(m_handler->get(), linearIndex), m_handler));
}

const matioCpp::Variable matioCpp::Variable::getCellElement(size_t linearIndex) const
{
    return Variable(matioCpp::WeakMatvar(Mat_VarGetCell(m_handler->get(), linearIndex), m_handler));
}

size_t matioCpp::Variable::getStructNumberOfFields() const
{
    return Mat_VarGetNumberOfFields(m_handler->get());
}

char * const * matioCpp::Variable::getStructFields() const
{
    return Mat_VarGetStructFieldnames(m_handler->get());
}

size_t matioCpp::Variable::getStructFieldIndex(const std::string &field) const
{
    size_t i = 0;
    size_t numberOfFields = getStructNumberOfFields();
    char * const * fields = getStructFields();

    if (!fields)
    {
        return numberOfFields;
    }

    while (i < numberOfFields && (strcmp(fields[i], field.c_str()) != 0))
    {
        ++i;
    }

    return i;
}

bool matioCpp::Variable::setStructField(size_t index, const matioCpp::Variable &newValue, size_t structPositionInArray)
{
    if (!m_handler->isShared())
    {
        std::cerr << "[ERROR][matioCpp::Variable::setStructField] Cannot set the field if the variable is not owning the memory." << std::endl;
        return false;
    }

    Variable copiedNonOwning(matioCpp::WeakMatvar(matioCpp::MatvarHandler::GetMatvarDuplicate(newValue.toMatio()), m_handler));
    if (!copiedNonOwning.isValid())
    {
        return false;
    }

    matvar_t* previousField = Mat_VarSetStructFieldByIndex(m_handler->get(), index, structPositionInArray, copiedNonOwning.toMatio());

    m_handler->dropOwnedPointer(previousField); //This avoids that any variable that was using this pointer before tries to access it.
    MatvarHandler::DeleteMatvar(previousField);

    return Mat_VarGetStructFieldByIndex(m_handler->get(), index, structPositionInArray);
}

bool matioCpp::Variable::addStructField(const std::string &newField)
{
    if (m_handler->isShared()) //This means that the variable is not part of an array
    {
        int err = Mat_VarAddStructField(m_handler->get(), newField.c_str());

        if (err)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool matioCpp::Variable::setStructField(const matioCpp::Variable &newValue, size_t structPositionInArray)
{
    size_t fieldindex = getStructFieldIndex(newValue.name());

    if ((fieldindex == getStructNumberOfFields()) && !((getArrayNumberOfElements() == 1) && addStructField(newValue.name())))
    {
        //This is the case when the field has not been found and, either there are more than one elements (i.e. it is part of an array), or there was an error in adding the field
        return false;
    }

    return setStructField(fieldindex, newValue, structPositionInArray);
}

matioCpp::Variable matioCpp::Variable::getStructField(size_t index, size_t structPositionInArray)
{
    return Variable(matioCpp::WeakMatvar(Mat_VarGetStructFieldByIndex(m_handler->get(), index, structPositionInArray), m_handler));
}

const matioCpp::Variable matioCpp::Variable::getStructField(size_t index, size_t structPositionInArray) const
{
    return Variable(matioCpp::WeakMatvar(Mat_VarGetStructFieldByIndex(m_handler->get(), index, structPositionInArray), m_handler));
}

matioCpp::Struct matioCpp::Variable::getStructArrayElement(size_t linearIndex)
{
    size_t numberOfFields = getStructNumberOfFields();
    std::vector<matvar_t*> fields(numberOfFields + 1, nullptr);
    for (size_t field = 0; field < numberOfFields; ++field)
    {
        fields[field] = Mat_VarGetStructFieldByIndex(m_handler->get(), field, linearIndex);
    }
    std::string newName = name() + std::to_string(linearIndex);
    size_t dimensions[] = {1,1};
    matvar_t* rawStruct = Mat_VarCreate(newName.c_str(), matio_classes::MAT_C_STRUCT, matio_types::MAT_T_STRUCT, 2, dimensions, fields.data(), MAT_F_DONT_COPY_DATA);
    return matioCpp::Struct(matioCpp::WeakMatvar(rawStruct, m_handler, matioCpp::DeleteMode::Delete));
}

const matioCpp::Struct matioCpp::Variable::getStructArrayElement(size_t linearIndex) const
{
    size_t numberOfFields = getStructNumberOfFields();
    std::vector<matvar_t*> fields(numberOfFields + 1, nullptr);
    for (size_t field = 0; field < numberOfFields; ++field)
    {
        fields[field] = Mat_VarGetStructFieldByIndex(m_handler->get(), field, linearIndex);
    }
    std::string newName = name() + "_" + std::to_string(linearIndex);
    size_t dimensions[] = {1,1};
    matvar_t* rawStruct = Mat_VarCreate(newName.c_str(), matio_classes::MAT_C_STRUCT, matio_types::MAT_T_STRUCT, 2, dimensions, fields.data(), MAT_F_DONT_COPY_DATA);
    return matioCpp::Struct(matioCpp::WeakMatvar(rawStruct, m_handler, matioCpp::DeleteMode::Delete));
}

bool matioCpp::Variable::checkCompatibility(const matvar_t *inputPtr, matioCpp::VariableType, matioCpp::ValueType) const
{
    return inputPtr;
}

matioCpp::Variable::Variable()
    : m_handler(new matioCpp::SharedMatvar())
{

}

matioCpp::Variable::Variable(const matvar_t *inputVar)
    : m_handler(new matioCpp::SharedMatvar())
{
    m_handler->duplicateMatvar(inputVar);
}

matioCpp::Variable::Variable(const matioCpp::Variable &other)
    : m_handler(new matioCpp::SharedMatvar())
{
    if (other.isValid())
    {
        m_handler->duplicateMatvar(other.toMatio());
    }
}

matioCpp::Variable::Variable(matioCpp::Variable &&other)
{
    m_handler = other.m_handler;
    other.m_handler = nullptr;
}

matioCpp::Variable::Variable(const MatvarHandler &handler)
    : m_handler(handler.pointerToDuplicate())
{

}

matioCpp::Variable::~Variable()
{
    if (m_handler)
    {
        delete m_handler;
    }
    m_handler = nullptr;
}

matioCpp::Variable &matioCpp::Variable::operator=(const matioCpp::Variable &other)
{
    bool ok = fromOther(other);
    assert(ok);
    matioCpp::unused(ok);
    return *this;
}

matioCpp::Variable &matioCpp::Variable::operator=(matioCpp::Variable &&other)
{
    bool ok = fromOther(std::forward<matioCpp::Variable>(other));
    assert(ok);
    matioCpp::unused(ok);
    return *this;
}

bool matioCpp::Variable::fromMatio(const matvar_t *inputVar)
{
    if (!inputVar)
    {
        std::cerr << "[matioCpp::Variable::fromMatio] The input pointer is null." << std::endl;
        return false;
    }

    matioCpp::VariableType outputVariableType = matioCpp::VariableType::Unsupported;
    matioCpp::ValueType outputValueType = matioCpp::ValueType::UNSUPPORTED;
    get_types_from_matvart(inputVar, outputVariableType, outputValueType);

    if (!checkCompatibility(inputVar, outputVariableType, outputValueType))
    {
        return false;
    }

    return m_handler->duplicateMatvar(inputVar);
}

bool matioCpp::Variable::fromOther(const matioCpp::Variable &other)
{
    return fromMatio(other.toMatio());
}

bool matioCpp::Variable::fromOther(matioCpp::Variable &&other)
{
    if (!other.isValid())
    {
        std::cerr << "[matioCpp::Variable::fromOther] The input variable is not valid." << std::endl;
        return false;
    }

    if (!checkCompatibility(other.toMatio(), other.variableType(), other.valueType()))
    {
        return false;
    }

    if (m_handler)
    {
        delete m_handler;
    }
    m_handler = other.m_handler;
    other.m_handler = nullptr;
    return true;
}

const matvar_t *matioCpp::Variable::toMatio() const
{
    return m_handler->get();
}

matvar_t *matioCpp::Variable::toMatio()
{
    return m_handler->get();
}

std::string matioCpp::Variable::name() const
{
    if (isValid())
    {
        return m_handler->get()->name;
    }
    else
    {
        return "";
    }
}

matioCpp::VariableType matioCpp::Variable::variableType() const
{
    return m_handler->variableType();
}

matioCpp::ValueType matioCpp::Variable::valueType() const
{
    return m_handler->valueType();
}

bool matioCpp::Variable::isComplex() const
{
    if (isValid())
    {
        return m_handler->get()->isComplex;
    }
    else
    {
        return false;
    }
}

matioCpp::Span<const size_t> matioCpp::Variable::dimensions() const
{
    if (isValid())
    {
        return matioCpp::make_span(m_handler->get()->dims, m_handler->get()->rank);
    }
    else
    {
        return matioCpp::Span<const size_t>();
    }
}

bool matioCpp::Variable::isValid() const
{
    return m_handler->get() && checkCompatibility(m_handler->get(), m_handler->variableType(), m_handler->valueType());
}

matioCpp::CellArray matioCpp::Variable::asCellArray()
{
    return matioCpp::CellArray(*m_handler);
}

const matioCpp::CellArray matioCpp::Variable::asCellArray() const
{
    return matioCpp::CellArray(*m_handler);
}

matioCpp::Struct matioCpp::Variable::asStruct()
{
    return matioCpp::Struct(*m_handler);
}

const matioCpp::Struct matioCpp::Variable::asStruct() const
{
    return matioCpp::Struct(*m_handler);
}

matioCpp::StructArray matioCpp::Variable::asStructArray()
{
    return matioCpp::StructArray(*m_handler);
}

const matioCpp::StructArray matioCpp::Variable::asStructArray() const
{
    return matioCpp::StructArray(*m_handler);
}
