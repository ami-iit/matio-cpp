#ifndef MATIOCPP_MATVARHANDLER_H
#define MATIOCPP_MATVARHANDLER_H

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

#include <matioCpp/ForwardDeclarations.h>

class matioCpp::MatvarHandler
{
protected:

    class PointerInfo
    {
        matvar_t* m_ptr; /** The matvar_t pointer. **/

        VariableType m_varType{VariableType::Unsupported}; /** The corresponding variable type. **/

        ValueType m_valueType{ValueType::UNSUPPORTED}; /** The corresponding value type. **/

        DeleteMode m_mode; /** The deletion mode. **/

    public:

        /**
         * @brief Default constructor
         */
        PointerInfo();

        /**
         * @brief Constructor
         * @param ptr The input pointer
         * @param deleteMode The deletion mode
         */
        PointerInfo(matvar_t* ptr, DeleteMode deleteMode);

        /**
         * Destructor
         */
        ~PointerInfo();

        /**
         * @brief Change the input pointer
         * @param ptr The new pointer
         * @param deleteMode The corresponding deletion mode
         */
        void changePointer(matvar_t* ptr, DeleteMode deleteMode);

        /**
         * @brief Delete the matvar pointer
         */
        void deletePointer();

        /**
         * @brief Get the matvar pointer
         * @return The inner matvar pointer.
         */
        matvar_t* pointer();

        /**
         * @brief Get the variable type
         * @return the variable type.
         */
        VariableType variableType() const;

        /**
         * @brief Get the value type
         * @return the value type.
         */
        ValueType valueType() const;

        /**
         * @brief Get the deletion mode
         * @return the deletion mode.
         */
        DeleteMode deleteMode() const;

        /**
         * @brief Comparison operator
         * @param other The pointer to compare
         * @return True if input pointer is different from the other input pointer.
         */
        bool operator!=(const PointerInfo& other) const;

        /**
         * @brief Delete the given pointer given the mode
         * @param ptr The pointer to delete
         * @param deleteMode The deletion mode
         */
        static void DeletePointer(matvar_t *ptr, DeleteMode deleteMode);
    };

    struct Dependency
    {
        std::unordered_set<matvar_t*> dependencies; /** The set of dependencies. **/

        matioCpp::DeleteMode mode; /** Deletion mode for the dependency. **/

        matvar_t* parent{nullptr}; /** The parent of the pointer. Null if there is no parent (or if the parent is the main pointer) **/
    };

    /**
     * @brief The Ownership class is used to define the ownership of a matvar. SharedMatvar and WeakMatvar have a shared_ptr and a weak_ptr to it respectively.
     */
    class Ownership
    {
        std::weak_ptr<PointerInfo> m_main; /** A pointer to the main PointerInfo that contains the pointer to be freed when the corresponding ownership is deallocated. It is the one owning the pointers in the other two sets. **/

        std::unordered_map<matvar_t*, Dependency> m_dependencyTree; /** A map that links a pointer to its Dependency object. **/

        /**
         * @brief Drops all the dependencies of a given pointer
         * @param previouslyOwned The pointer whose dependencies have to be dropped.
         */
        void dropDependencies(matvar_t* previouslyOwned);

    public:

        /**
         * @brief Constructor
         * @param pointerToDeallocate A weak pointer toward the PointerInfo that contains the pointer to be freed.
         */
        Ownership(std::weak_ptr<PointerInfo> pointerToDeallocate);

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
         * @param owner The owner of owned. It is necessary to define the dependency tree.
         * @param mode Define how the owned variable has to be deleted
         */
        void own(matvar_t* owned, const MatvarHandler* owner, matioCpp::DeleteMode mode);

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
     * @brief Shared pointer to a PointerInfo. This allows sharing the same matvar_t across several objects.
     *
     * Since it is separate from the ownership, it is possible to avoid deallocating this pointer if necessary (e.g. when getting an element from a struct).
     */
    std::shared_ptr<PointerInfo> m_ptr;

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
     * @param deleteMode The mode with which the pointer has to be deleted
     */
    MatvarHandler(matvar_t *inputPtr, matioCpp::DeleteMode deleteMode = matioCpp::DeleteMode::Delete);

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
     * @brief Get the value type of the pointer
     * @return The value type of the pointer
     */
    ValueType valueType() const;

    /**
     * @brief Get the variable type of the pointer
     * @return The variable type of the pointer
     */
    VariableType variableType() const;

    /**
     * @brief Get a duplicate of the input matvar pointer/
     * @param inputPtr The input pointer
     * @return A copy of the pointer
     */
    static matvar_t* GetMatvarDuplicate(const matvar_t* inputPtr);

    /**
     * @brief Delete the specified Matvar
     * @param pointerToDelete The matvar to be deleted
     * @param mode The deletion mode
     */
    static void DeleteMatvar(matvar_t* pointerToDelete, DeleteMode mode = DeleteMode::Delete);

};


#endif // MATIOCPP_MATVARHANDLER_H
