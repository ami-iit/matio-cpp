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

    /**
     * @brief The Ownership struct is an empty struct used to define the ownership of a matvar. SharedMatvar and WeakMatvar have a shared_ptr and a weak_ptr to it respectively.
     */
    struct Ownership
    { };

    /**
     * @brief Shared pointer to a matvar_t pointer. This allows sharing the same matvar_t across several objects.
     *
     * Since it is separate from the ownership, it is possible to avoid deallocating the matvar_t pointer if necessary (e.g. when getting an element from a struct).
     */
    std::shared_ptr<matvar_t*> m_ptr;

public:

    /**
     * @brief Default destructor
     */
    virtual ~MatvarHandler(){}

    /**
     * @brief Get the shared matvar_t  pointer.
     * @return the shared matvar_t pointer. It may be null if matvar_t has never been allocated.
     */
    virtual matvar_t* get() const = 0;

    /**
     * @brief Check if the onwership is shared with the current object.
     * @return True if the current object is sharing the ownership of the matvar_t.
     */
    virtual bool isShared() const  = 0;

    /**
     * @brief Perform a deep copy of the input pointer.
     * @param inputPtr The input pointer
     * @return True if successfull, false otherwise (e.g. if isShared is false, or the input pointer is null).
     */
    virtual bool duplicateMatvar(matvar_t* inputPtr) = 0;

};


#endif // MATIOCPP_MATVARHANDLER_H
