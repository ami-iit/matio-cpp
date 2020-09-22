#ifndef MATIOCPP_MATVARHANDLER_H
#define MATIOCPP_MATVARHANDLER_H

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

class matioCpp::MatvarHandler
{
protected:

    struct Ownership
    { };

    std::shared_ptr<matvar_t*> m_ptr;

public:

    virtual ~MatvarHandler(){}

    virtual matvar_t* get() const = 0;

    virtual bool isShared() const  = 0;

    virtual bool duplicateMatvar(matvar_t* inputPtr) = 0;

};


#endif // MATIOCPP_MATVARHANDLER_H
