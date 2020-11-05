/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

#include <matioCpp/WeakMatvar.h>

matioCpp::WeakMatvar::WeakMatvar()
{

}

matioCpp::WeakMatvar::WeakMatvar(const matioCpp::WeakMatvar &other)
    : matioCpp::MatvarHandler(other)
    , m_ownership(other.m_ownership)
{

}

matioCpp::WeakMatvar::WeakMatvar(matioCpp::WeakMatvar &&other)
    : matioCpp::MatvarHandler(other)
    , m_ownership(other.m_ownership)
{

}

matioCpp::WeakMatvar::WeakMatvar(const SharedMatvar &other)
{
   matioCpp::WeakMatvar otherWeak = other.weakOwnership();
   m_ownership = otherWeak.m_ownership;
   m_ptr = otherWeak.m_ptr;
}

matioCpp::WeakMatvar::WeakMatvar(matvar_t *inputPtr, const SharedMatvar &owner, DeleteMode mode)
    : matioCpp::MatvarHandler(inputPtr)
    , m_ownership(owner.ownership())

{
    auto locked = m_ownership.lock();
    if (locked)
    {
        locked->own(inputPtr, &owner, mode);
    }
}

matioCpp::WeakMatvar::WeakMatvar(matvar_t *inputPtr, const matioCpp::MatvarHandler *owner, DeleteMode mode)
{
    m_ownership = owner->weakOwnership().m_ownership;
    m_ptr = std::make_shared<matvar_t*>(inputPtr);

    auto locked = m_ownership.lock();
    if (locked)
    {
        locked->own(inputPtr, owner, mode);
    }
}

matioCpp::WeakMatvar::~WeakMatvar()
{

}

matvar_t *matioCpp::WeakMatvar::get() const
{
    assert(m_ptr);
    auto locked = m_ownership.lock();

    if (locked && locked->isOwning(*m_ptr))
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

bool matioCpp::WeakMatvar::importMatvar(matvar_t *)
{
    std::cerr << "[ERROR][matioCpp::WeakMatvar::importMatvar] Cannot import inputPtr. A WeakMatvar cannot modify the matvar pointer." << std::endl;

    return false;
}

matioCpp::MatvarHandler *matioCpp::WeakMatvar::pointerToDuplicate() const
{
    return new WeakMatvar(*this);
}

matioCpp::WeakMatvar matioCpp::WeakMatvar::weakOwnership() const
{
    return *this;
}

void matioCpp::WeakMatvar::dropOwnedPointer(matvar_t *previouslyOwnedPointer)
{
    auto locked = m_ownership.lock();
    if (locked)
    {
        locked->drop(previouslyOwnedPointer);
    }
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
    return operator=(other.weakOwnership());
}

