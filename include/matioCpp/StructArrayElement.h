#ifndef MATIOCPP_STRUCTARRAYELEMENT_H
#define MATIOCPP_STRUCTARRAYELEMENT_H

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
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

#include "impl/StructArrayElement.tpp"

#endif // MATIOCPP_STRUCTARRAYELEMENT_H
