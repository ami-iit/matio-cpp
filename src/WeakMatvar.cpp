/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

#include <matioCpp/WeakMatvar.h>

matioCpp::WeakMatvar::WeakMatvar()
{
    m_ptr = std::make_shared<matvar_t*>(nullptr);
}

matioCpp::WeakMatvar::WeakMatvar(const matioCpp::WeakMatvar &other)
    : m_ownership(other.m_ownership)
{
    m_ptr = other.m_ptr;
}

matioCpp::WeakMatvar::WeakMatvar(matioCpp::WeakMatvar &&other)
    : m_ownership(other.m_ownership)
{
    m_ptr = other.m_ptr;
}

matioCpp::WeakMatvar::WeakMatvar(const SharedMatvar &other)
{
    m_ownership = other.ownership();
    m_ptr = other.m_ptr;
}

matioCpp::WeakMatvar::WeakMatvar(matvar_t *inputPtr, const SharedMatvar &owner)
{
    m_ownership = owner.ownership();
    m_ptr = std::make_shared<matvar_t*>(inputPtr);
}

matioCpp::WeakMatvar::~WeakMatvar()
{
}

matvar_t *matioCpp::WeakMatvar::get() const
{
    assert(m_ptr);
    auto locked = m_ownership.lock();

    if (locked)
    {
        return *m_ptr;
    }
    else
    {
        return nullptr;
    }
}

bool matioCpp::WeakMatvar::isShared() const
{
    return false;
}

bool matioCpp::WeakMatvar::duplicateMatvar(const matvar_t *)
{
    std::cerr << "[ERROR][matioCpp::WeakMatvar::duplicateFromMatio] Cannot duplicate from inputPtr. A WeakMatvar cannot modify the matvar pointer." << std::endl;

    return false;
}

matioCpp::WeakMatvar &matioCpp::WeakMatvar::operator=(const matioCpp::WeakMatvar &other)
{
    m_ownership = other.m_ownership;
    m_ptr = other.m_ptr;
    return *this;
}

matioCpp::WeakMatvar &matioCpp::WeakMatvar::operator=(matioCpp::WeakMatvar &&other)
{
    m_ownership = other.m_ownership;
    m_ptr = other.m_ptr;
    return *this;
}

matioCpp::WeakMatvar &matioCpp::WeakMatvar::operator=(const SharedMatvar &other)
{
    m_ownership = other.m_ownership;
    m_ptr = other.m_ptr;
    return *this;
}

