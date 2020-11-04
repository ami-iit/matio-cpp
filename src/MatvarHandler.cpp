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

void matioCpp::MatvarHandler::Ownership::dropDependencies(matvar_t *previouslyOwned)
{
    if (!previouslyOwned)
    {
        return;
    }

    std::unordered_map<matvar_t*, Dependency>::iterator it = m_dependencyTree.find(previouslyOwned);

    if (it == m_dependencyTree.end())
    {
        return;
    }

    for (matvar_t* child : it->second.dependencies)
    {
        dropDependencies(child);
    }

    if (it->second.mode == matioCpp::DeleteMode::Delete)
    {
        Mat_VarFree(previouslyOwned);
    }

    m_dependencyTree.erase(previouslyOwned);
}

matioCpp::MatvarHandler::Ownership::Ownership(std::weak_ptr<matvar_t *> pointerToDeallocate)
    : m_main(pointerToDeallocate)
{

}

matioCpp::MatvarHandler::Ownership::~Ownership()
{
    dropAll();
}

bool matioCpp::MatvarHandler::Ownership::isOwning(matvar_t *test)
{
    return (test && ((test == *(m_main.lock())) || (m_dependencyTree.find(test) != m_dependencyTree.end())));
}

void matioCpp::MatvarHandler::Ownership::own(matvar_t *owned, const MatvarHandler *owner, DeleteMode mode)
{
    assert(owner);
    if (owned)
    {
        Dependency dep;
        dep.mode = mode;
        if (*(owner->m_ptr) != *(m_main.lock()))
        {
            dep.parent = *(owner->m_ptr);

            assert(m_dependencyTree.find(dep.parent) != m_dependencyTree.end());

            m_dependencyTree[dep.parent].dependencies.insert(owned);
        }

        m_dependencyTree[owned] = dep;
    }
}

void matioCpp::MatvarHandler::Ownership::drop(matvar_t *previouslyOwned)
{
    if (!previouslyOwned)
    {
        return;
    }

    std::unordered_map<matvar_t*, Dependency>::iterator it = m_dependencyTree.find(previouslyOwned);

    if (it == m_dependencyTree.end())
    {
        return;
    }

    std::unordered_map<matvar_t*, Dependency>::iterator parent = m_dependencyTree.find(it->second.parent);

    if ((it->second.parent != *(m_main.lock())) && (parent != m_dependencyTree.end()))
    {
        parent->second.dependencies.erase(previouslyOwned);
    }

    dropDependencies(previouslyOwned);

    m_dependencyTree.erase(previouslyOwned);
}

void matioCpp::MatvarHandler::Ownership::dropAll()
{
    std::shared_ptr<matvar_t*> locked = m_main.lock();
    if (locked)
    {
        if (*locked)
        {
            Mat_VarFree(*locked);
            *locked = nullptr;
        }
    }

    for(std::pair<matvar_t*, Dependency> dep : m_dependencyTree)
    {
        if (dep.second.mode == matioCpp::DeleteMode::Delete)
        {
            Mat_VarFree(dep.first);
        }
    }

    m_dependencyTree.clear();
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

    if (outputVariableType == matioCpp::VariableType::CellArray) // It is a different case because Mat_VarDuplicate segfaults with a CellArray
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
            // See https://github.com/tbeu/matio/issues/158
        Mat_VarFree(shallowCopy);
    }
    else if ((outputVariableType == matioCpp::VariableType::Struct) || (outputVariableType == matioCpp::VariableType::StructArray))
    {
        matvar_t * shallowCopy = Mat_VarDuplicate(inputPtr, 0); // Shallow copy to remove const

        size_t totalElements = 1;

        for (int i = 0; i < inputPtr->rank; ++i)
        {
            totalElements *= inputPtr->dims[i];
        }

        size_t numberOfFields = Mat_VarGetNumberOfFields(shallowCopy);

        std::vector<matvar_t*> vectorOfPointers(totalElements * numberOfFields + 1, nullptr); //The vector of pointers has to be nullptr terminated.

        size_t innerIndex = 0;
        for (size_t i = 0; i < totalElements; ++i)
        {
            for (size_t field = 0; field < numberOfFields; ++field)
            {
                matvar_t* internalPointer = Mat_VarGetStructFieldByIndex(shallowCopy, field, i);
                if (internalPointer)
                {
                    vectorOfPointers[innerIndex] = GetMatvarDuplicate(internalPointer); //Deep copy
                }
                innerIndex++;
            }
        }
        assert(innerIndex == totalElements * numberOfFields);

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
