#ifndef MATIOCPP_VECTOR_H
#define MATIOCPP_VECTOR_H

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */


#include <matioCpp/ForwardDeclarations.h>
#include <matioCpp/ConversionUtilities.h>
#include <matioCpp/Span.h>
#include <matioCpp/Variable.h>
#include <matioCpp/VectorIterator.h>

/**
 * @brief Vector is a particular type of Variable specialized for 1-D arrays of a generic type T.
 */
template<typename T>
class matioCpp::Vector : public matioCpp::Variable
{

    /**
     * @brief Private utility method to initialize the Vector from existing data.
     * @param name The name of the Variable
     * @param inputVector A span to an existing vector.
     * @return true in case of success.
     */
    bool initializeVector(const std::string& name, Span<const T> inputVector);

    /**
     * @brief Check if an input matio pointer is compatible with the vector class.
     * @param inputPtr The input matvar_t pointer.
     * @param variableType The type of variable.
     * @param valueType The value type.
     * @return True if compatible. False otherwise, throwing errors.
     */
    virtual bool checkCompatibility(const matvar_t* inputPtr, matioCpp::VariableType variableType, matioCpp::ValueType valueType) const final;

public:

    using element_type = T; /** Defines the type of an element of the Vector. Needed to use the iterator. **/

    using value_type = std::remove_cv_t<T>; /** Defines the type of an element of the Vector without "const". Useful to use make_span. **/

    using allocator_type = std::allocator<T>; /** Defines how to allocate T. Useful to use make_span. **/

    using index_type = size_t; /** The type used for indices. **/

    using reference = element_type&; /** The reference type. **/

    using pointer = typename std::allocator_traits<std::allocator<T>>::pointer; /** The pointer type. **/

    using const_pointer = typename std::allocator_traits<std::allocator<T>>::const_pointer; /** The const pointer type. **/

    using iterator = vector_iterator<Vector<T>, false>; /** The iterator type. **/

    using const_iterator = vector_iterator<Vector<T>, true>; /** The const iterator type. **/

    using reverse_iterator = std::reverse_iterator<iterator>; /** The reverse iterator type. **/

    using const_reverse_iterator =std::reverse_iterator<const_iterator>; /** The const reverse iterator type. **/

    /**
     * @brief Default Constructor
     * @note The name is set to "unnamed_vector".
     */
    Vector();

    /**
     * @brief Constructor
     * @param name The name of the Vector
     * @note If the type is char, the name is also copied in the content.
     */
    Vector(const std::string& name);

    /**
     * @brief Constructor
     * @param name The name of the Vector
     * @param inputVector The input data.
     */
    Vector(const std::string& name, index_type dimensions);

    /**
     * @brief Constructor
     * @param name The name of the Vector
     * @param inputVector The input data.
     */
    template<typename in = T, typename = typename std::enable_if<!std::is_same<T, const char>::value>::type>
    Vector(const std::string& name, Span<const T> inputVector);

    /**
     * @brief Constructor
     * @param name The name of the Vector
     * @param inputString The input string.
     */
    Vector(const std::string& name, const std::string& inputString);

    /**
     * @brief Copy constructor
     */
    Vector(const Vector<T>& other);

    /**
     * @brief Move constructor
     */
    Vector(Vector<T>&& other);

    /**
     * @brief Constructor to share the data ownership of another variable.
     * @param handler The MatvarHandler handler to the matvar_t which has to be shared.
     */
    Vector(const MatvarHandler& handler);

    /**
    * Destructor.
    */
    ~Vector();

    /**
     * @brief Assignement operator (copy) from another Vector.
     * @param other The other Vector.
     * @note Also the name is copied
     * @return A reference to this Vector.
     */
    Vector<T>& operator=(const Vector<T>& other);

    /**
     * @brief Assignement operator (move) from another Vector.
     * @param other The other Vector.
     * @note Also the name is copied.
     * @return A reference to this Vector.
     */
    Vector<T>& operator=(Vector<T>&& other);

    /**
     * @brief Assignement operator from another Vector.
     * @param other The input span.
     * @return A reference to this Vector.
     */
    Vector<T>& operator=(const Span<T>& other);

    /**
     * @brief Assignement operator from another string.
     * @param other The input string.
     * @note This is available only if the type is char.
     * @return A reference to this Vector.
     */
    Vector<T>& operator=(const std::string& other);

    /**
     * @brief Get this Vector as a Span
     */
    matioCpp::Span<T> toSpan();

    /**
     * @brief Get this Vector as a Span (const version)
     */
    const matioCpp::Span<const T> toSpan() const;

    /**
     * @brief Change the name of the Variable
     * @param newName The new name
     * @return True if successful.
     */
    bool setName(const std::string& newName);

    /**
     * @brief Get the size of the vector.
     * @return The size of the Vector.
     */
    index_type size() const;

    /**
     * @brief Resize the vector.
     * @param newSize The new size.
     *
     * @warning This requires to allocate memory for twice the new size.
     */
    void resize(index_type newSize);

    /**
     * @brief Clear the vector
     */
    void clear();

    /**
     * @brief Direct access to the underlying array.
     * @return A pointer to the internal data.
     */
    pointer data();

    /**
     * @brief Direct access to the underlying array.
     * @return A pointer to the internal data.
     */
    const_pointer data() const;

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning el has to be structly smaller than size.
     * @return A reference to the element.
     */
    reference operator()(index_type el);

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning el has to be structly smaller than size.
     * @return A copy to the element.
     */
    value_type operator()(index_type el) const;

    /**
     * @brief Get the Vector as a string
     * @return A string that copies the internal data.
     * @note This is available only if the type is char.
     */
    std::string operator()() const;

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning el has to be structly smaller than size.
     * @return A reference to the element.
     */
    reference operator[](index_type el);

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning el has to be structly smaller than size.
     * @return A copy to the element.
     */
    value_type operator[](index_type el) const;

    /**
     * @brief begin Iterator
     * @return An iterator to the beginning of the sequence
     */
    iterator begin();

    /**
     * @brief end Iterator
     * @return An iterator to the end of the sequence
     * @warning This element acts as a placeholder; attempting to access it results in undefined behavior.
     */
    iterator end();

    /**
     * @brief begin Iterator
     * @return A const iterator to the end of the sequence
     */
    const_iterator begin() const;

    /**
     * @brief end Iteratorspan
     * @return A const iterator to the end of the sequence
     * @warning This element acts as a placeholder; attempting to access it results in undefined behavior.
     */
    const_iterator end() const;

    /**
     * @brief cbegin Iterator
     * @return A const iterator to the end of the sequence
     */
    const_iterator cbegin() const;

    /**
     * @brief cend Iterator
     * @return A const iterator to the end of the sequence
     * @warning This element acts as a placeholder; attempting to access it results in undefined behavior.
     */
    const_iterator cend() const;

    /**
     * @brief rbegin Iterator to the first element of the reversed vector
     * @return Returns a reverse iterator to the first element of the reversed vector.
     * It corresponds to the last element of the non-reversed vector
     */
    reverse_iterator rbegin();

    /**
     * @brief rend Iterator to the element following the last element of the reversed vector.
     * @return Returns a reverse iterator to the element following the last element of the reversed vector.
     *  It corresponds to the element preceding the first element of the non-reversed vector.
     * @warning This element acts as a placeholder; attempting to access it results in undefined behavior.
     */
    reverse_iterator rend();

    /**
     * @brief rbegin Iterator to the first element of the reversed vector
     * @return Returns a const reverse iterator to the first element of the reversed vector.
     * It corresponds to the last element of the non-reversed vector
     */
    const_reverse_iterator rbegin() const;

    /**
     * @brief rend Iterator to the element following the last element of the reversed vector.
     * @return Returns a reverse iterator to the element following the last element of the reversed vector.
     *  It corresponds to the element preceding the first element of the non-reversed vector.
     * @warning This element acts as a placeholder; attempting to access it results in undefined behavior.
     */
    const_reverse_iterator rend() const;

    /**
     * @brief crbegin Iterator to the first element of the reversed vector
     * @return Returns a const reverse iterator to the first element of the reversed vector.
     * It corresponds to the last element of the non-reversed vector
     */
    const_reverse_iterator crbegin() const;

    /**
     * @brief crend Iterator to the element following the last element of the reversed vector.
     * @return Returns a reverse iterator to the element following the last element of the reversed vector.
     *  It corresponds to the element preceding the first element of the non-reversed vector.
     * @warning This element acts as a placeholder; attempting to access it results in undefined behavior.
     */
    const_reverse_iterator crend() const;
};

#include "impl/Vector.tpp"

#endif // MATIOCPP_VECTOR_H
