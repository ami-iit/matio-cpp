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

    /**
     * @brief Shared pointer to the Ownership object.
     */
    std::shared_ptr<MatvarHandler::Ownership> m_ownership;

public:

    /**
     * @brief Default constructor.
     *
     * It initialize the matvar_t to nullptr and it is the only owner of it.
     */
    SharedMatvar();

    /**
     * @brief Copy constructor
     * @param other The other object from which to copy.
     */
    SharedMatvar(const SharedMatvar& other);

    /**
     * @brief Move constructor
     * @param other The other object from which to get the context.
     */
    SharedMatvar(SharedMatvar&& other);

    /**
     * @brief Import an existing matvar_t and take control of it (it will be deallocated when no other SharedMatvar objects point to it).
     * @param inputPtr The input pointer to control.
     */
    SharedMatvar(matvar_t* inputPtr);

    /**
     * @brief Destructor
     */
    ~SharedMatvar();

    /**
     * Docs inherited
     */
    virtual matvar_t* get() const final;

    /**
     * Docs inherited
     */
    virtual bool isShared() const final;

    /**
     * Docs inherited
     */
    virtual bool duplicateMatvar(const matvar_t *inputPtr) final;

    /**
     * Docs inherited
     */
    virtual MatvarHandler* pointerToDuplicate() const final;

    /**
     * Docs inherited
     */
    virtual WeakMatvar weakOwnership() const final;

    /**
     * @brief Copy assignement
     * @param other The other object to copy.
     * @return A reference to the current object.
     */
    SharedMatvar& operator=(const SharedMatvar& other);

    /**
     * @brief Move assignement
     * @param other The other object to get the context.
     * @return A reference to the current object.
     */
    SharedMatvar& operator=(SharedMatvar&& other);

    /**
     * @brief Get a weak_ptr to the ownership
     * @return A weak_ptr to the ownership.
     */
    std::weak_ptr<MatvarHandler::Ownership> ownership() const;

};

#endif // MATIOCPP_SHAREDMATVAR_H
