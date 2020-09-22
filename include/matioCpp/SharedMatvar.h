#ifndef MATIOCPP_SHAREDMATVAR_H
#define MATIOCPP_SHAREDMATVAR_H

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


class matioCpp::SharedMatvar : public matioCpp::MatvarHandler
{

    std::shared_ptr<MatvarHandler::Ownership> m_ownership;

    friend class WeakMatvar;

public:

    SharedMatvar();

    SharedMatvar(const SharedMatvar& other);

    SharedMatvar(SharedMatvar&& other);

    SharedMatvar(matvar_t* inputPtr);

    ~SharedMatvar();

    virtual matvar_t* get() const final;

    virtual bool isShared() const final;

    virtual bool duplicateMatvar(matvar_t* inputPtr) final;

    SharedMatvar& operator=(const SharedMatvar& other);

    SharedMatvar& operator=(SharedMatvar&& other);

    std::weak_ptr<MatvarHandler::Ownership> ownership() const;

};

#endif // MATIOCPP_SHAREDMATVAR_H
