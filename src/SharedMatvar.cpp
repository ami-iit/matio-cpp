/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

#include <matioCpp/SharedMatvar.h>
#include <matioCpp/WeakMatvar.h>
#include <matioCpp/Span.h>
#include <matioCpp/ConversionUtilities.h>


matioCpp::SharedMatvar::SharedMatvar()
    : matioCpp::MatvarHandler()
    , m_ownership(std::make_shared<MatvarHandler::Ownership>(m_ptr))
{

}

matioCpp::SharedMatvar::SharedMatvar(const matioCpp::SharedMatvar &other)
    : matioCpp::MatvarHandler(other)
    , m_ownership(other.m_ownership)
{

}

matioCpp::SharedMatvar::SharedMatvar(matioCpp::SharedMatvar &&other)
    : matioCpp::MatvarHandler(other)
    , m_ownership(other.m_ownership)
{

}

matioCpp::SharedMatvar::SharedMatvar(matvar_t *inputPtr)
    : matioCpp::MatvarHandler(inputPtr)
    , m_ownership(std::make_shared<MatvarHandler::Ownership>(m_ptr))
{

}

matioCpp::SharedMatvar::~SharedMatvar()
{

}

matvar_t *matioCpp::SharedMatvar::get() const
{
    assert(m_ptr);
    return m_ptr->pointer();
}

bool matioCpp::SharedMatvar::isShared() const
{
    return true;
}

bool matioCpp::SharedMatvar::duplicateMatvar(const matvar_t *inputPtr)
{
    return importMatvar(matioCpp::MatvarHandler::GetMatvarDuplicate(inputPtr));
}

bool matioCpp::SharedMatvar::importMatvar(matvar_t *inputPtr)
{
    assert(m_ptr);

    m_ownership->dropAll();

    m_ptr->changePointer(inputPtr, DeleteMode::Delete);

    return true;
}

matioCpp::MatvarHandler *matioCpp::SharedMatvar::pointerToDuplicate() const
{
    return new SharedMatvar(*this);
}

matioCpp::WeakMatvar matioCpp::SharedMatvar::weakOwnership() const
{
    matioCpp::WeakMatvar weak;
    weak.m_ownership = m_ownership;
    weak.m_ptr = m_ptr;
    return weak;
}

void matioCpp::SharedMatvar::dropOwnedPointer(matvar_t *previouslyOwnedPointer)
{
    m_ownership->drop(previouslyOwnedPointer);
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

matioCpp::SharedMatvar matioCpp::SharedMatvar::GetMatvarShallowDuplicate(const matvar_t *inputPtr)
{
    SharedMatvar output;
    output.m_ptr = std::make_shared<PointerInfo>(Mat_VarDuplicate(inputPtr, 0), DeleteMode::ShallowDelete);
    output.m_ownership = std::make_shared<MatvarHandler::Ownership>(output.m_ptr);
    return output;
}
