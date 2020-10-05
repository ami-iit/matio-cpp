/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

#include <matioCpp/MatvarHandler.h>
#include <matioCpp/ConversionUtilities.h>

matioCpp::MatvarHandler::Ownership::Ownership(std::weak_ptr<matvar_t *> ponterToDeallocate)
    : m_pointerToDeallocate(ponterToDeallocate)
{

}

matioCpp::MatvarHandler::Ownership::~Ownership()
{
    std::shared_ptr<matvar_t*> locked = m_pointerToDeallocate.lock();
    if (locked)
    {
        if (*locked)
        {
            Mat_VarFree(*locked);
            *locked = nullptr;
        }
    }
}

matioCpp::MatvarHandler::MatvarHandler()
    : m_ptr(std::make_shared<matvar_t*>(nullptr))
{
}

matioCpp::MatvarHandler::MatvarHandler(matvar_t *inputPtr)
    : m_ptr(std::make_shared<matvar_t*>(inputPtr))
{

}

matioCpp::MatvarHandler::MatvarHandler(const matioCpp::MatvarHandler &other)
    : m_ptr(other.m_ptr)
{

}

matioCpp::MatvarHandler::MatvarHandler(matioCpp::MatvarHandler &&other)
    : m_ptr(other.m_ptr)
{

}

matvar_t *matioCpp::MatvarHandler::GetMatvarDuplicate(const matvar_t *inputPtr)
{
    std::string errorPrefix = "[ERROR][matioCpp::MatvarHandler::GetMatvarDuplicate] ";
    if (!inputPtr)
    {
        return nullptr;
    }

    VariableType outputVariableType;
    ValueType outputValueType;
    matvar_t* outputPtr;

    if (!matioCpp::get_types_from_matvart(inputPtr, outputVariableType, outputValueType))
    {
        std::cerr << errorPrefix << "The inputPtr is not supported." << std::endl;
        return nullptr;
    }

    if (outputVariableType == matioCpp::VariableType::CellArray) // It is a different case because Mat_VarDuplicate segafaults with a CellArray
    {
        matvar_t * shallowCopy = Mat_VarDuplicate(inputPtr, 0); // Shallow copy to remove const

        size_t totalElements = 1;

        for (int i = 0; i < inputPtr->rank; ++i)
        {
            totalElements *= inputPtr->dims[i];
        }

        std::vector<matvar_t*> vectorOfPointers(totalElements, nullptr);
        for (size_t i = 0; i < totalElements; ++i)
        {
            matvar_t* internalPointer = Mat_VarGetCell(shallowCopy, i);
            if (internalPointer)
            {
                vectorOfPointers[i] = GetMatvarDuplicate(internalPointer); //Deep copy
            }
        }

        outputPtr = Mat_VarCreate(inputPtr->name, inputPtr->class_type, inputPtr->data_type, inputPtr->rank, inputPtr->dims, vectorOfPointers.data(), 0);

        shallowCopy->data = nullptr; //This is a workaround for what it seems a matio problem.
            //When doing a shallow copy, the data is not copied,
            //but it will try to free it anyway with Mat_VarFree.
            //This avoids matio to attempt freeing the data
        Mat_VarFree(shallowCopy);
    }
    else
    {
        outputPtr = Mat_VarDuplicate(inputPtr, 1); //0 Shallow copy, 1 Deep copy
    }

    return outputPtr;

}
