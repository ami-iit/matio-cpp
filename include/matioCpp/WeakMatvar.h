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
    /**
     * @brief A weak_ptr to the Ownership object.
     */
    std::weak_ptr<MatvarHandler::Ownership> m_ownership;

    friend class matioCpp::SharedMatvar;

public:

    /**
     * @brief Default constructor.
     *
     * This objects points to no matvar_t.
     */
    WeakMatvar();

    /**
     * @brief Copy constructor.
     * @param other The other object from which to copy.
     */
    WeakMatvar(const WeakMatvar& other);

    /**
     * @brief Move constructor.
     * @param other The other object from which to get the context.
     */
    WeakMatvar(WeakMatvar&& other);

    /**
     * @brief Construct a WeakMatvar from a SharedMatvar.
     *
     * The WeakMatvar object will have a weak_ptr to the same Ownership
     * object of SharedMatvar.
     * This object will point (in a shared way) to the same matvar_t pointer.
     * The content of matvar_t may be deallocated if all the SharedMatvar
     * objects are deallocated.
     * @param other The input SharedMatvar object.
     */
    WeakMatvar(const SharedMatvar& other);

    /**
     * @brief Import an already existing matvar_t pointer which is owned by owner.
     *
     * owner needs to make sure that inputPtr is deallocated.
     * @param inputPtr The matvar_t pointer to import
     * @param owner The owner of inputPtr (the one that takes care of deallocating it)
     */
    WeakMatvar(matvar_t* inputPtr, const SharedMatvar& owner);

    /**
     * @brief Import an already existing matvar_t pointer which is owned by owner.
     *
     * owner needs to make sure that inputPtr is deallocated.
     * @param inputPtr The matvar_t pointer to import
     * @param owner The owner of inputPtr (the one that takes care of deallocating it)
     *
     * @note An assertion is thrown if owner is not shared
     */
    WeakMatvar(matvar_t* inputPtr, const MatvarHandler* owner);

    /**
     * Destructor
     */
    ~WeakMatvar();

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
     *
     * This always returns false.
     */
    virtual bool duplicateMatvar(const matvar_t*) final;

    /**
     * Docs inherited
     *
     * This always returns false.
     */
    virtual bool importMatvar(matvar_t *) final;

    /**
     * Docs inherited
     */
    virtual MatvarHandler* pointerToDuplicate() const final;

    /**
     * Docs inherited
     */
    virtual WeakMatvar weakOwnership() const final;

    /**
     * Docs inherited
     */
    virtual void dropOwnedPointer(matvar_t* previouslyOwnedPointer) final;

    /**
     * @brief Copy assignement
     * @param other The other object to copy.
     * @return A reference to the current object.
     */
    WeakMatvar& operator=(const WeakMatvar& other);

    /**
     * @brief Move assignement
     * @param other The other object to get the context.
     * @return A reference to the current object.
     */
    WeakMatvar& operator=(WeakMatvar&& other);

    /**
     * @brief Assignement operator from a SharedMatvar
     * @param other The SharedMatvar from which to share the ownership and import the matvar_t pointer.
     * @return A reference to the current object.
     */
    WeakMatvar& operator=(const SharedMatvar& other);

};

#endif // MATIOCPP_WEAKMATVAR_H
