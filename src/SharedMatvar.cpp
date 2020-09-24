/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

#include <matioCpp/SharedMatvar.h>
#include <matioCpp/Span.h>


matioCpp::SharedMatvar::SharedMatvar()
    : m_ownership(std::make_shared<MatvarHandler::Ownership>())
{
    m_ptr = std::make_shared<matvar_t*>(nullptr);
}

matioCpp::SharedMatvar::SharedMatvar(const matioCpp::SharedMatvar &other)
    : m_ownership(other.m_ownership)
{
    m_ptr = other.m_ptr;
}

matioCpp::SharedMatvar::SharedMatvar(matioCpp::SharedMatvar &&other)
    : m_ownership(other.m_ownership)
{
    m_ptr = other.m_ptr;
}

matioCpp::SharedMatvar::SharedMatvar(matvar_t *inputPtr)
    : m_ownership(std::make_shared<MatvarHandler::Ownership>())
{
    m_ptr = std::make_shared<matvar_t*>(inputPtr);
}

matioCpp::SharedMatvar::~SharedMatvar()
{
    if (m_ownership.use_count() == 1) //This object is the last one pointing to the same matvar
    {
        if (m_ptr)
        {
            if (*m_ptr)
            {
                Mat_VarFree(*m_ptr);
            }
            *m_ptr = nullptr;
        }
    }

}

matvar_t *matioCpp::SharedMatvar::get() const
{
    assert(m_ptr);
    return *m_ptr;
}

bool matioCpp::SharedMatvar::isShared() const
{
    return true;
}

bool matioCpp::SharedMatvar::duplicateMatvar(const matvar_t *inputPtr)
{
    std::string errorPrefix = "[ERROR][matioCpp::SharedMatvar::duplicateFromMatio] ";
    if (!inputPtr)
    {
        std::cerr << errorPrefix << "Empty input variable." << std::endl;
        return false;
    }

    assert(m_ptr);

    if (*m_ptr)
    {
        Mat_VarFree(*m_ptr);
    }

    *m_ptr = Mat_VarDuplicate(inputPtr, 1); //0 Shallow copy, 1 Deep copy

    return true;
}

matioCpp::SharedMatvar &matioCpp::SharedMatvar::operator=(const matioCpp::SharedMatvar &other)
{
    m_ownership = other.m_ownership;
    m_ptr = other.m_ptr;
    return *this;
}

matioCpp::SharedMatvar &matioCpp::SharedMatvar::operator=(matioCpp::SharedMatvar &&other)
{
    m_ownership = other.m_ownership;
    m_ptr = other.m_ptr;
    return *this;
}

std::weak_ptr<matioCpp::MatvarHandler::Ownership> matioCpp::SharedMatvar::ownership() const
{
    return m_ownership;
}
