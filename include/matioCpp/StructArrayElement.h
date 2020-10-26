#ifndef MATIOCPP_STRUCTARRAYELEMENT_H
#define MATIOCPP_STRUCTARRAYELEMENT_H

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
#include <matioCpp/Variable.h>
#include <matioCpp/Struct.h>

template <bool isConst>
class matioCpp::StructArrayElement
{

public:

    using index_type = size_t; /** The type used for indices. **/

    using input_vector_type = std::conditional_t<isConst, const matioCpp::StructArray, matioCpp::StructArray>;

    using output_variable_type = std::conditional_t<isConst, const matioCpp::Variable, matioCpp::Variable>;

    using output_struct_type = std::conditional_t<isConst, const matioCpp::Struct, matioCpp::Struct>;

    /**
     * @brief Element
     * @param index
     * @param array
     */
    StructArrayElement(index_type index, input_vector_type* array);

    /**
     * @brief Deleted default constructor
     */
    StructArrayElement() = delete;

    /**
     * @brief Copy operator
     * @param other The Element to be copied
     * @note The content of the element is not copied
     */
    StructArrayElement(const StructArrayElement<isConst>& other);

    /**
     * @brief Copy operator
     * @param other The Element to be copied
     * @note The content of the element is not copied
     */
    template<bool B, typename = typename std::enable_if_t<!B && isConst>>
    StructArrayElement(const StructArrayElement<B>& other);

    /**
     * @brief Move operator
     * @param other The Element to be moved
     * @note The content of the element is not copied
     */
    StructArrayElement(matioCpp::StructArrayElement<isConst> &&other);

    /**
     * @brief Move operator
     * @param other The Element to be moved
     * @note The content of the element is not copied
     */
    template<bool B, typename = typename std::enable_if_t<!B && isConst>>
    StructArrayElement(matioCpp::StructArrayElement<B> &&other);

    /**
     * @brief Deleted copy assignement since it is not clear whether the index or the content have to be copied
     */
    StructArrayElement& operator=(const StructArrayElement& other) = delete;

    /**
     * @brief Move assignement
     * @param other The other Element.
     * @note No data is copied
     * @return A reference to this Element.
     */
    StructArrayElement<isConst>& operator=(StructArrayElement<isConst>&& other);

    /**
     * @brief Move assignement (from non-const to const)
     * @param other The other Element.
     * @note No data is copied
     * @return A reference to this Element.
     */
    template<bool B, typename = typename std::enable_if_t<!B && isConst>>
    StructArrayElement<isConst>& operator=(StructArrayElement<B>&& other);

    /**
     * @brief Assignement operator from a struct
     * @param other The struct to be copied
     * @note The whole struct is copied
     * @return A reference to this Element.
     */
    template<bool B = isConst, typename = typename std::enable_if_t<(B == isConst) && !B>>
    StructArrayElement& operator=(const matioCpp::Struct& other) const;

    /**
     * @brief Set from a vector of Variables
     * @param elements The elements to be set.
     * @note Previous fields are lost
     * @return True if successful. False otherwise, for example if one of the input variables is not valid or if the fields do not match the already existing fields.
     */
    template<bool B = isConst, typename = typename std::enable_if_t<(B == isConst) && !B>>
    bool fromVectorOfVariables(const std::vector<Variable> &elements) const;

    /**
     * @brief Get the total number of fields in the struct
     * @return The total number of fields
     */
    index_type numberOfFields() const;

    /**
     * @brief Get the list of fields
     * @return The list of fields.
     */
    std::vector<std::string> fields() const;

    /**
     * @brief Check if a field is existing
     * It performs a linear search over the output of fields().
     * @return True if the field is existing, false otherwise.
     */
    bool isFieldExisting(const std::string& field) const;

    /**
     * @brief Get the index of the specified field in the struct by performing a linear search
     * @param field The field to search
     * @return The index of the field, the output of numberOfFields() if not found.
     */
    size_t getFieldIndex(const std::string& field) const;

    /**
     * @brief Set the field at the specified position
     * @param index The linear index of the specified field
     * @param newValue The Variable that will be copied in the specified location
     * @return True if successful, false otherwise (for example if the newValue is not valid)
     * @note An assertion is thrown if index is out of bounds, but only in debug mode
     * @note The name is of newValue is not considered.
     */
    template<bool B = isConst, typename = typename std::enable_if_t<(B == isConst) && !B>>
    bool setField(index_type index, const Variable& newValue) const;

    /**
     * @brief Set the field given the newValue name
     * @param newValue The Variable that will be copied in the specified field
     * @return True if successful, false otherwise (for example if the newValue is not valid or the field is not found)
     */
    template<bool B = isConst, typename = typename std::enable_if_t<(B == isConst) && !B>>
    bool setField(const Variable& newValue) const;

    /**
     * @brief Get the element as a Struct.
     * @note This requires memory allocation. In particular, it needs to allocate a number of pointers equal to the number of fields
     * @return A Struct with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the variable cannot be resized and the name cannot change.
     */
    output_struct_type asStruct() const;

    /**
     * @brief Access field at a specific index.
     * @param el The index of the field to be accessed.
     * @return A Variable with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the variable cannot be resized and the name cannot change.
     * @note An assertion is thrown if index is out of bounds, but only in debug mode
     */
    output_variable_type operator()(index_type el) const;

    /**
     * @brief Access field at a specific index.
     * @param el The name of the field to be accessed.
     * @return A Variable with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the variable cannot be resized and the name cannot change.
     * @note If the field is not found, the output variable is not valid.
     */
    output_variable_type operator()(const std::string& el) const;

    /**
     * @brief Access field at a specific index.
     * @param el The index of the field to be accessed.
     * @return A Variable with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the variable cannot be resized and the name cannot change.
     * @note An assertion is thrown if index is out of bounds, but only in debug mode
     */
    output_variable_type operator[](index_type el) const;

    /**
     * @brief Access field at a specific index.
     * @param el The name of the field to be accessed.
     * @return A Variable with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the variable cannot be resized and the name cannot change.
     * @note If the field is not found, the output variable is not valid.
     */
    output_variable_type operator[](const std::string& el) const;

private:

    index_type m_innerIndex; /** The linear index in the array. **/

    input_vector_type* m_array; /** The pointer to the array **/

    friend class StructArrayElement<true>;
};

//---------------- Implementations

template <bool isConst>
matioCpp::StructArrayElement<isConst>::StructArrayElement(index_type index, input_vector_type *array)
    : m_innerIndex(index)
    , m_array(array)
{
    assert(m_array);
}

template <bool isConst>
matioCpp::StructArrayElement<isConst>::StructArrayElement(const matioCpp::StructArrayElement<isConst> &other)
    : m_innerIndex(other.m_innerIndex)
    , m_array(other.m_array)
{
    assert(m_array);
}

template <bool isConst>
template<bool B, typename >
matioCpp::StructArrayElement<isConst>::StructArrayElement(const matioCpp::StructArrayElement<B> &other)
    : m_innerIndex(other.m_innerIndex)
    , m_array(other.m_array)
{
    static_assert (!B && isConst, "This constructor is meant to be used to obtain a const element from a non const one.");
    assert(m_array);
}

template <bool isConst>
matioCpp::StructArrayElement<isConst>::StructArrayElement(matioCpp::StructArrayElement<isConst> &&other)
    : m_innerIndex(other.m_innerIndex)
    , m_array(other.m_array)
{
    assert(m_array);
}

template <bool isConst>
template<bool B, typename >
matioCpp::StructArrayElement<isConst>::StructArrayElement(matioCpp::StructArrayElement<B> &&other)
    : m_innerIndex(other.m_innerIndex)
    , m_array(other.m_array)
{
    static_assert (!B && isConst, "This constructor is meant to be used to obtain a const element from a non const one.");
    assert(m_array);
}

template <bool isConst>
matioCpp::StructArrayElement<isConst> &matioCpp::StructArrayElement<isConst>::operator=(matioCpp::StructArrayElement<isConst> &&other)
{
    m_innerIndex = other.m_innerIndex;
    m_array = other.m_array;
    assert(m_array);
    return *this;
}

template <bool isConst>
template<bool B, typename >
matioCpp::StructArrayElement<isConst> &matioCpp::StructArrayElement<isConst>::operator=(matioCpp::StructArrayElement<B> &&other)
{
    static_assert (!B && isConst, "This operator is meant to be used to obtain a const element from a non const one.");

    m_innerIndex = other.m_innerIndex;
    m_array = other.m_array;
    assert(m_array);
    return *this;
}

template <bool isConst>
template<bool B, typename >
matioCpp::StructArrayElement<isConst> &matioCpp::StructArrayElement<isConst>::operator=(const matioCpp::Struct &other) const
{
    static_assert (!B && isConst, "This operator is meant to be used to obtain a const element from a non const one.");

    bool ok = m_array->setElement(m_innerIndex, other);
    assert(ok && "Failed to set the specified element to the input struct.");
    matioCpp::unused(ok);

    return *this;
}

template <bool isConst>
template<bool B, typename >
bool matioCpp::StructArrayElement<isConst>::fromVectorOfVariables(const std::vector<Variable> &elements) const
{
    static_assert ((B == isConst) && !B, "This method can be used only if the the element is not const.");

    char * const * arrayFields = m_array->getStructFields();

    if (m_array->numberOfFields() != elements.size())
    {
        std::cerr << "[ERROR][matioCpp::StructArrayElement::fromVectorOfVariables] The input vector is supposed to have size equal to the number of fields of the struct array." <<std::endl;
        assert(false);
        return *this;
    }

    for (size_t i = 0; i < m_array->numberOfFields(); ++i)
    {
        if (strcmp(arrayFields[i], elements[i].name().c_str()) != 0)
        {
            std::cerr << "[ERROR][matioCpp::StructArrayElement::operator=] The name " << elements[i].name().c_str() << " of the input vector of variables at position "
                      << std::to_string(i) << " is supposed to be " << arrayFields[i]
                      << ". Cannot insert in a struct array a new field in a single element." <<std::endl;
            assert(false);
            return *this;
        }

        bool ok = m_array->setStructField(i, elements[i], m_innerIndex);
        if (!ok)
        {
            std::cerr << "[ERROR][matioCpp::StructArrayElement::operator=] Failed to set field " << arrayFields[i] << "." <<std::endl;
            assert(false);
            return *this;
        }
    }

    return *this;
}

template <bool isConst>
typename matioCpp::StructArrayElement<isConst>::index_type matioCpp::StructArrayElement<isConst>::numberOfFields() const
{
    return m_array->numberOfFields();
}

template <bool isConst>
std::vector<std::string> matioCpp::StructArrayElement<isConst>::fields() const
{
    return m_array->fields();
}

template <bool isConst>
bool matioCpp::StructArrayElement<isConst>::isFieldExisting(const std::string &field) const
{
    return m_array->isFieldExisting(field);
}

template <bool isConst>
size_t matioCpp::StructArrayElement<isConst>::getFieldIndex(const std::string &field) const
{
    return m_array->getFieldIndex(field);
}

template <bool isConst>
template<bool B, typename >
bool matioCpp::StructArrayElement<isConst>::setField(index_type index, const Variable &newValue) const
{
    static_assert ((B == isConst) && !B, "This method can be used only if the the element is not const.");

    assert(index < numberOfFields() && "The index is out of bounds.");
    if (!newValue.isValid())
    {
        std::cerr << "[ERROR][matioCpp::StructArrayElement::setField] The input variable is not valid." << std::endl;
        return false;
    }

    return m_array->setStructField(index, newValue, m_innerIndex);
}

template <bool isConst>
template<bool B, typename >
bool matioCpp::StructArrayElement<isConst>::setField(const Variable &newValue) const
{
    static_assert ((B == isConst) && !B, "This method can be used only if the the element is not const.");

    if (!newValue.isValid())
    {
        std::cerr << "[ERROR][matioCpp::StructArrayElement::setField] The input variable is not valid." << std::endl;
        return false;
    }

    size_t index = getFieldIndex(newValue.name());
    if (index == numberOfFields())
    {
        std::cerr << "[ERROR][matioCpp::StructArrayElement::setField] No field named " << newValue.name() << "." << std::endl;
        return false;
    }

    return setField(index, newValue);
}

template <bool isConst>
typename matioCpp::StructArrayElement<isConst>::output_struct_type matioCpp::StructArrayElement<isConst>::asStruct() const
{
    return m_array->getStructArrayElement(m_innerIndex);
}

template <bool isConst>
typename matioCpp::StructArrayElement<isConst>::output_variable_type matioCpp::StructArrayElement<isConst>::operator ()(index_type el) const
{
    assert(el < numberOfFields() && "The specified index is out of bounds");
    return m_array->getStructField(el, m_innerIndex);
}

template <bool isConst>
typename matioCpp::StructArrayElement<isConst>::output_variable_type matioCpp::StructArrayElement<isConst>::operator ()(const std::string &el) const
{
    size_t index = getFieldIndex(el);
    assert(index < numberOfFields() && "The specified field does not exist.");
    return m_array->getStructField(index, m_innerIndex);
}

template <bool isConst>
typename matioCpp::StructArrayElement<isConst>::output_variable_type matioCpp::StructArrayElement<isConst>::operator [](index_type el) const
{
    assert(el < numberOfFields() && "The specified index is out of bounds");
    return m_array->getStructField(el, m_innerIndex);
}

template <bool isConst>
typename matioCpp::StructArrayElement<isConst>::output_variable_type matioCpp::StructArrayElement<isConst>::operator [](const std::string &el) const
{
    size_t index = getFieldIndex(el);
    assert(index < numberOfFields() && "The specified field does not exist.");
    return m_array->getStructField(index, m_innerIndex);
}

#endif // MATIOCPP_STRUCTARRAYELEMENT_H
