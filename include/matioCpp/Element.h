#ifndef MATIOCPP_ELEMENT_H
#define MATIOCPP_ELEMENT_H

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
     * @return True if compatible. False otherwise, throwing errors.
     */
    virtual bool checkCompatibility(const matvar_t* inputPtr) const final;

public:

    using element_type = T; /** Defines the type of the Element. **/

    using value_type = std::remove_cv_t<T>; /** Defines the type of the Element. without "const". **/

    using reference = element_type&; /** The reference type. **/

    using pointer = typename std::allocator_traits<std::allocator<T>>::pointer; /** The pointer type. **/

    using const_pointer = typename std::allocator_traits<std::allocator<T>>::const_pointer; /** The const pointer type. **/

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
    Element(const std::string& name, T inputValue);

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
    Element<T>& operator=(T value);

    /**
     * @brief Change the name of the Variable
     * @param newName The new name
     * @return True if successfull.
     *
     * @warning This requires the Element to be reallocated. It performs memory allocation.
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
    operator T() const;

};

#include "impl/Element.tpp"


#endif // MATIOCPP_ELEMENT_H
