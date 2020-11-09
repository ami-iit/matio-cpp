#ifndef MATIOCPP_ELEMENT_H
#define MATIOCPP_ELEMENT_H

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

#include <matioCpp/ForwardDeclarations.h>
#include <matioCpp/ConversionUtilities.h>
#include <matioCpp/Variable.h>

/**
 * @brief Element allows having a 1x1 variable (like double, int,..) castable to a primitive type.
 */
template<typename T>
class matioCpp::Element : public matioCpp::Variable
{

    /**
     * @brief Check if an input matio pointer is compatible with the Element class.
     * @param inputPtr The input matvar_t pointer.
     * @param variableType The type of variable.
     * @param valueType The value type.
     * @return True if compatible. False otherwise, throwing errors.
     */
    virtual bool checkCompatibility(const matvar_t* inputPtr, matioCpp::VariableType variableType, matioCpp::ValueType valueType) const final;

public:

    using type = T; /** Defines the type specified in the template. **/

    using element_type = typename get_type<T>::type; /** Defines the type of the Element. **/

    using value_type = std::remove_cv_t<element_type>; /** Defines the type of the Element. without "const". **/

    using reference = element_type&; /** The reference type. **/

    using pointer = typename std::allocator_traits<std::allocator<element_type>>::pointer; /** The pointer type. **/

    using const_pointer = typename std::allocator_traits<std::allocator<element_type>>::const_pointer; /** The const pointer type. **/

    /**
     * @brief Default Constructor
     * @note The name is set to "unnamed_element".
     */
    Element();

    /**
     * @brief Constructor
     * @param name The name of the Element
     */
    Element(const std::string& name);

    /**
     * @brief Constructor
     * @param name The name of the Element
     * @param inputValue The value to be given to the element
     */
    Element(const std::string& name, element_type inputValue);

    /**
     * @brief Copy constructor
     */
    Element(const Element<T>& other);

    /**
     * @brief Move constructor
     */
    Element(Element<T>&& other);

    /**
     * @brief Constructor to share the data ownership of another variable.
     * @param handler The MatvarHandler handler to the matvar_t which has to be shared.
     */
    Element(const MatvarHandler& handler);

    /**
    * Destructor.
    */
    ~Element();

    /**
     * @brief Assignement operator (copy) from another Element.
     * @param other The other Element.
     * @note Also the name is copied
     * @return A reference to this Element.
     */
    Element<T>& operator=(const Element<T>& other);

    /**
     * @brief Assignement operator (move) from another Element.
     * @param other The other Element.
     * @note Also the name is copied
     * @return A reference to this Element.
     */
    Element<T>& operator=(Element<T>&& other);

    /**
     * @brief Assignement operator (copy) from an element of type T.
     * @param other The other Element.
     * @return A reference to this Element.
     */
    Element<T>& operator=(element_type value);

    /**
     * @brief Change the name of the Variable
     * @param newName The new name
     * @return True if successful.
     */
    bool setName(const std::string& newName);

    /**
     * @brief Access the Element.
     * @return A reference to the element.
     */
    reference operator()();

    /**
     * @brief Access the Element.
     * @return A copy to the element.
     */
    value_type operator()() const;

    /**
     * @brief Casting operator to T
     */
    operator element_type() const;

};

#include "impl/Element.tpp"


#endif // MATIOCPP_ELEMENT_H
