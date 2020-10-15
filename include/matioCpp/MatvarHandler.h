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
     * @brief The Ownership class is used to define the ownership of a matvar. SharedMatvar and WeakMatvar have a shared_ptr and a weak_ptr to it respectively.
     */
    class Ownership
    {
        std::weak_ptr<matvar_t*> m_main; /** A pointer to the main matvar_t* that needs to be freed when the corresponding ownership is deallocated. It is the one owning the pointers in the other two sets. **/

        std::unordered_set<matvar_t*> m_otherPointersToDeallocate; /** The set of owned pointers that also need to be deallocated. **/

        std::unordered_set<matvar_t*> m_ownedPointers;  /** The set of pointers, owned by the handler. **/
    public:

        /**
         * @brief Constructor
         * @param ponterToDeallocate A weak pointer toward the matvar_t* that needs to be freed.
         */
        Ownership(std::weak_ptr<matvar_t*> pointerToDeallocate);

        /**
         * @brief Destructor
         */
        ~Ownership();

        /**
         * @brief Check if an input pointer is owned by this ownership object
         * @param test The pointer to test
         * @return true if the ownership objects owns the pointer
         */
        bool isOwning(matvar_t* test);

        /**
         * @brief Add a pointer to the list of owned pointers.
         * This will not be deallocated as it is supposed to be part of the matvar_t pointer stored in m_ptr.
         * @param owned The pointer to be considered owned.
         */
        void own(matvar_t* owned, matioCpp::DeleteMode mode);

        /**
         * @brief Drop a previously owned pointer and deleted if necessary
         * @param previouslyOwned The pointer that is not own anymore
         */
        void drop(matvar_t* previouslyOwned);

        /**
         * @brief Drops all the previously owned pointers and free those that need to be deallocated, including the main one.
         */
        void dropAll();
    };

    /**
     * @brief Shared pointer to a matvar_t pointer. This allows sharing the same matvar_t across several objects.
     *
     * Since it is separate from the ownership, it is possible to avoid deallocating this pointer if necessary (e.g. when getting an element from a struct).
     */
    std::shared_ptr<matvar_t*> m_ptr;

public:

    /**
     * @brief Default constructor
     *
     * Initializes matvar_t* to nullptr
     */
    MatvarHandler();

    /**
     * @brief Constructor from an already existing matvar_t pointer
     * @param inputPtr The input matvar_t pointer
     */
    MatvarHandler(matvar_t *inputPtr);

    /**
     * @brief Copy constructor
     * @param other The other MatvarHandler to copy
     */
    MatvarHandler(const MatvarHandler& other);

    /**
     * @brief Move constructor
     * @param other The other MatvarHandler to move
     */
    MatvarHandler(MatvarHandler&& other);

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
     * @return True if successful, false otherwise (e.g. if isShared is false, or the input pointer is null).
     */
    virtual bool duplicateMatvar(const matvar_t* inputPtr) = 0;

    /**
     * @brief Import the input pointer.
     * @param inputPtr The input pointer
     * @return True if successful, false otherwise (e.g. if isShared is false, or the input pointer is null).
     */
    virtual bool importMatvar(matvar_t* inputPtr) = 0;

    /**
     * @brief Get a pointer to a duplicate of the MatvarHandler. The pointed object has to be deallocated manually
     * @return A pointer to a copy of the MatvarHandler.
     */
    virtual MatvarHandler* pointerToDuplicate() const = 0;

    /**
     * @brief Get a WeakMatvar version of the current MatvarHandler
     * @return A WeakMatvar version of the current MatvarHandler.
     */
    virtual WeakMatvar weakOwnership() const = 0;

    /**
     * @brief Drop a pointer from the list of owned pointers.
     * @param previouslyOwnedPointer The pointer that is not owned anymore
     */
    virtual void dropOwnedPointer(matvar_t* previouslyOwnedPointer) = 0;

    /**
     * @brief Get a duplicate of the input matvar pointer/
     * @param inputPtr The input pointer
     * @return A copy of the pointer
     */
    static matvar_t* GetMatvarDuplicate(const matvar_t* inputPtr);

};


#endif // MATIOCPP_MATVARHANDLER_H
