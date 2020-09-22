#ifndef MATIOCPP_WEAKMATVAR_H
#define MATIOCPP_WEAKMATVAR_H

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

#include <matioCpp/ForwardDeclarations.h>
#include <matioCpp/MatvarHandler.h>
#include <matioCpp/SharedMatvar.h>

class matioCpp::WeakMatvar : public matioCpp::MatvarHandler
{
    std::weak_ptr<MatvarHandler::Ownership> m_ownership;

public:

    WeakMatvar();

    WeakMatvar(const WeakMatvar& other);

    WeakMatvar(WeakMatvar&& other);

    WeakMatvar(const SharedMatvar& other);

    WeakMatvar(matvar_t* inputPtr, const SharedMatvar& owner);

    ~WeakMatvar();

    virtual matvar_t* get() const final;

    virtual bool isShared() const final;

    virtual bool duplicateMatvar(matvar_t*) final;

    WeakMatvar& operator=(const WeakMatvar& other);

    WeakMatvar& operator=(WeakMatvar&& other);

    WeakMatvar& operator=(const SharedMatvar& other);

};

#endif // MATIOCPP_WEAKMATVAR_H
