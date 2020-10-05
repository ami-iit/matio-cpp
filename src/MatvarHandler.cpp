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
