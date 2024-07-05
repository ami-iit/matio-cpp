#ifndef MATIOCPP_STRUCT_H
#define MATIOCPP_STRUCT_H

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

#include <matioCpp/ForwardDeclarations.h>
#include <matioCpp/Variable.h>

/**
 * @brief Struct is a particular type of Variable specialized for structs.
 */
class matioCpp::Struct : public matioCpp::Variable
{

    /**
     * @brief Check if an input matio pointer is compatible with the Struct class.
     * @param inputPtr The input matvar_t pointer.
     * @param variableType The type of variable.
     * @param valueType The value type.
     * @return True if compatible. False otherwise, throwing errors.
     */
    virtual bool checkCompatibility(const matvar_t* inputPtr, matioCpp::VariableType variableType, matioCpp::ValueType valueType) const final;

    friend class StructArray;

public:

    using index_type = size_t; /** The type used for indices. **/

    /**
     * @brief Default Constructor
     * @note The name is set to "unnamed_struct".
     */
    Struct();

    /**
     * @brief Constructor
     * @param name The name of the Struct
     */
    Struct(const std::string& name);

    /**
     * @brief Constructor
     * @param name The name of the Struct
     * @param elements The elements to be added to the Struct (they are copied)
     */
    Struct(const std::string& name, const std::vector<Variable> &elements);

    /**
     * @brief Copy constructor
     */
    Struct(const Struct& other);

    /**
     * @brief Move constructor
     */
    Struct(Struct&& other);

    /**
     * @brief Constructor to share the data ownership of another variable.
     * @param handler The MatvarHandler handler to the matvar_t which has to be shared.
     */
    Struct(const MatvarHandler& handler);

    /**
    * Destructor.
    */
    ~Struct();

    /**
     * @brief Assignement operator (copy) from another Struct.
     * @param other The other Struct.
     * @note Also the name is copied
     * @return A reference to this Struct.
     */
    Struct& operator=(const Struct& other);

    /**
     * @brief Assignement operator (move) from another Struct.
     * @param other The other Struct.
     * @note Also the name is copied
     * @return A reference to this Struct.
     */
    Struct& operator=(Struct&& other);

    /**
     * @brief Set from a vector of Variables
     * @param elements The elements to be set.
     * @note Previous fields are lost
     * @return True if successful. False otherwise, for example if one of the input variables is not valid.
     */
    bool fromVectorOfVariables(const std::vector<Variable> &elements);

    /**
     * @brief Change the name of the Struct
     * @param newName The new name
     * @return True if successful.
     */
    bool setName(const std::string& newName);

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
     * @brief Clear the struct
     */
    void clear();

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
     */
    bool setField(index_type index, const Variable& newValue);

    /**
     * @brief Set the given field to the newValue
     * @param field The name of the field to set
     * @param newValue The Variable that will be copied in the specified field
     * @return True if successful, false otherwise (for example if the newValue is not valid)
     * @note If the field is not found, a new field is created and appended to the struct.
     * @note The field is searched linearly. The name of newValue is not used
     */
    bool setField(const std::string& field, const Variable& newValue);

    /**
     * @brief Set the field given the newValue name
     * @param newValue The Variable that will be copied in the specified field
     * @return True if successful, false otherwise (for example if the newValue is not valid)
     * @note If the field is not found, a new field is created and appended to the struct.
     */
    bool setField(const Variable& newValue);

    /**
     * @brief Access field at a specific index.
     * @param el The index of the field to be accessed.
     * @return A Variable with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the variable cannot be resized and the name cannot change.
     * @note An assertion is thrown if index is out of bounds, but only in debug mode
     */
    matioCpp::Variable operator()(index_type el);

    /**
     * @brief Access field at a specific index.
     * @param el The index of the field to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A const Variable with a weak ownership to the underlying mat variable.
     * @note An assertion is thrown if index is out of bounds, but only in debug mode
     */
    const matioCpp::Variable operator()(index_type el) const;

    /**
     * @brief Access field at a specific index.
     * @param el The name of the field to be accessed.
     * @return A Variable with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the variable cannot be resized and the name cannot change.
     * @note If the field is not found, the output variable is not valid.
     */
    matioCpp::Variable operator()(const std::string& el);

    /**
     * @brief Access field at a specific index.
     * @param el The name of the field to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A const Variable with a weak ownership to the underlying mat variable.
     * @note If the field is not found, the output variable is not valid.
     */
    const matioCpp::Variable operator()(const std::string& el) const;

    /**
     * @brief Access field at a specific index.
     * @param el The index of the field to be accessed.
     * @return A Variable with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the variable cannot be resized and the name cannot change.
     * @note An assertion is thrown if index is out of bounds, but only in debug mode
     */
    matioCpp::Variable operator[](index_type el);

    /**
     * @brief Access field at a specific index.
     * @param el The index of the field to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A const Variable with a weak ownership to the underlying mat variable.
     * @note An assertion is thrown if index is out of bounds, but only in debug mode
     */
    const matioCpp::Variable operator[](index_type el) const;

    using Variable::operator[];

};

#endif // MATIOCPP_STRUCT_H
