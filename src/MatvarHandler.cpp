/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

#include <matioCpp/MatvarHandler.h>
#include <matioCpp/SharedMatvar.h>
#include <matioCpp/ConversionUtilities.h>

matioCpp::MatvarHandler::PointerInfo::PointerInfo()
{
    m_ptr = nullptr;
    m_mode = DeleteMode::DoNotDelete;
    m_varType = matioCpp::VariableType::Unsupported;
    m_valueType = matioCpp::ValueType::UNSUPPORTED;
}

matioCpp::MatvarHandler::PointerInfo::PointerInfo(matvar_t *ptr, DeleteMode deleteMode)
{
    changePointer(ptr, deleteMode);
}

matioCpp::MatvarHandler::PointerInfo::~PointerInfo()
{
    //It does nothing by default. The ownership determines when to delete the pointer
}

void matioCpp::MatvarHandler::PointerInfo::changePointer(matvar_t *ptr, DeleteMode deleteMode)
{
    //The previous pointer is not deleted since it is the ownership triggering it
    m_ptr = ptr;
    m_mode = deleteMode;
    m_varType = matioCpp::VariableType::Unsupported;
    m_valueType = matioCpp::ValueType::UNSUPPORTED;
    get_types_from_matvart(m_ptr, m_varType, m_valueType);
}

void matioCpp::MatvarHandler::PointerInfo::deletePointer()
{
    DeletePointer(m_ptr, m_mode);
    m_ptr = nullptr;
}

matvar_t *matioCpp::MatvarHandler::PointerInfo::pointer()
{
    return m_ptr;
}

matioCpp::VariableType matioCpp::MatvarHandler::PointerInfo::variableType() const
{
    return m_varType;
}

matioCpp::ValueType matioCpp::MatvarHandler::PointerInfo::valueType() const
{
    return m_valueType;
}

matioCpp::DeleteMode matioCpp::MatvarHandler::PointerInfo::deleteMode() const
{
    return m_mode;
}

bool matioCpp::MatvarHandler::PointerInfo::operator!=(const matioCpp::MatvarHandler::PointerInfo &other) const
{
    return m_ptr != other.m_ptr;
}

void matioCpp::MatvarHandler::PointerInfo::DeletePointer(matvar_t *ptr, DeleteMode deleteMode)
{
    if (deleteMode == DeleteMode::Delete || deleteMode == DeleteMode::ShallowDelete)
    {
        if (deleteMode == DeleteMode::ShallowDelete)
        {
            ptr->data = nullptr; //When doing a shallow copy, the data pointer is copied.
            //Hence, by freeing the shallowCopy, also the data would be deallocated.
            //This avoids matio to attempt freeing the data
            // See https://github.com/tbeu/matio/issues/158
        }
        Mat_VarFree(ptr);
    }
}

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

    PointerInfo::DeletePointer(previouslyOwned, it->second.mode);

    m_dependencyTree.erase(previouslyOwned);
}

matioCpp::MatvarHandler::Ownership::Ownership(std::weak_ptr<PointerInfo> pointerToDeallocate)
    : m_main(pointerToDeallocate)
{

}

matioCpp::MatvarHandler::Ownership::~Ownership()
{
    dropAll();
}

bool matioCpp::MatvarHandler::Ownership::isOwning(matvar_t *test)
{
    return (test && ((test == (m_main.lock()->pointer())) || (m_dependencyTree.find(test) != m_dependencyTree.end())));
}

void matioCpp::MatvarHandler::Ownership::own(matvar_t *owned, const matioCpp::MatvarHandler *owner, matioCpp::DeleteMode mode)
{
    assert(owner);
    if (owned)
    {
        Dependency dep;
        dep.mode = mode;
        if (*(owner->m_ptr) != *(m_main.lock()))
        {
            dep.parent = owner->m_ptr->pointer();

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

    if ((it->second.parent != m_main.lock()->pointer()) && (parent != m_dependencyTree.end()))
    {
        parent->second.dependencies.erase(previouslyOwned);
    }

    dropDependencies(previouslyOwned);

    m_dependencyTree.erase(previouslyOwned);
}

void matioCpp::MatvarHandler::Ownership::dropAll()
{
    std::shared_ptr<PointerInfo> locked = m_main.lock();
    if (locked)
    {
        locked->deletePointer();
    }

    for(std::pair<matvar_t*, Dependency> dep : m_dependencyTree)
    {
        PointerInfo::DeletePointer(dep.first, dep.second.mode);
    }

    m_dependencyTree.clear();
}

matioCpp::MatvarHandler::MatvarHandler()
    : m_ptr(std::make_shared<PointerInfo>())
{
}

matioCpp::MatvarHandler::MatvarHandler(matvar_t *inputPtr, DeleteMode deleteMode)
    : m_ptr(std::make_shared<PointerInfo>(inputPtr, deleteMode))
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

matioCpp::ValueType matioCpp::MatvarHandler::valueType() const
{
    return m_ptr->valueType();
}

matioCpp::VariableType matioCpp::MatvarHandler::variableType() const
{
    return m_ptr->variableType();
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
        SharedMatvar shallowCopy = SharedMatvar::GetMatvarShallowDuplicate(inputPtr); // Shallow copy to remove const

        size_t totalElements = 1;

        for (int i = 0; i < inputPtr->rank; ++i)
        {
            totalElements *= inputPtr->dims[i];
        }

        std::vector<matvar_t*> vectorOfPointers(totalElements, nullptr);
        for (size_t i = 0; i < totalElements; ++i)
        {
            matvar_t* internalPointer = Mat_VarGetCell(shallowCopy.get(), static_cast<int>(i));
            if (internalPointer)
            {
                vectorOfPointers[i] = GetMatvarDuplicate(internalPointer); //Deep copy
            }
        }

        outputPtr = Mat_VarCreate(inputPtr->name, inputPtr->class_type, inputPtr->data_type, inputPtr->rank, inputPtr->dims, vectorOfPointers.data(), 0);
    }
    else if ((outputVariableType == matioCpp::VariableType::Struct) || (outputVariableType == matioCpp::VariableType::StructArray))
    {
        SharedMatvar shallowCopy = SharedMatvar::GetMatvarShallowDuplicate(inputPtr); // Shallow copy to remove const

        size_t totalElements = 1;

        for (int i = 0; i < inputPtr->rank; ++i)
        {
            totalElements *= inputPtr->dims[i];
        }

        size_t numberOfFields = Mat_VarGetNumberOfFields(shallowCopy.get());

        std::vector<matvar_t*> vectorOfPointers(totalElements * numberOfFields + 1, nullptr); //The vector of pointers has to be nullptr terminated.

        size_t innerIndex = 0;
        for (size_t i = 0; i < totalElements; ++i)
        {
            for (size_t field = 0; field < numberOfFields; ++field)
            {
                matvar_t* internalPointer = Mat_VarGetStructFieldByIndex(shallowCopy.get(), field, i);
                if (internalPointer)
                {
                    vectorOfPointers[innerIndex] = GetMatvarDuplicate(internalPointer); //Deep copy
                }
                innerIndex++;
            }
        }
        assert(innerIndex == totalElements * numberOfFields);

        outputPtr = Mat_VarCreate(inputPtr->name, inputPtr->class_type, inputPtr->data_type, inputPtr->rank, inputPtr->dims, vectorOfPointers.data(), 0);
    }
    else
    {
        outputPtr = Mat_VarDuplicate(inputPtr, 1); //0 Shallow copy, 1 Deep copy
    }

    return outputPtr;

}

void matioCpp::MatvarHandler::DeleteMatvar(matvar_t *pointerToDelete, DeleteMode mode)
{
    PointerInfo::DeletePointer(pointerToDelete, mode);
}
