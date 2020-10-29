#ifndef MATIOCPP_CELLARRAY_H
#define MATIOCPP_CELLARRAY_H

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

#include <matioCpp/ForwardDeclarations.h>
#include <matioCpp/Variable.h>

/**
 * @brief CellArray is a particular type of Variable specialized for cell arrays.
 */
class matioCpp::CellArray : public matioCpp::Variable
{

    /**
     * @brief Check if an input matio pointer is compatible with the CellArray class.
     * @param inputPtr The input matvar_t pointer.
     * @param variableType The type of variable.
     * @param valueType The value type.
     * @return True if compatible. False otherwise, throwing errors.
     */
    virtual bool checkCompatibility(const matvar_t* inputPtr, matioCpp::VariableType variableType, matioCpp::ValueType valueType) const final;

public:

    using index_type = size_t; /** The type used for indices. **/

    /**
     * @brief Default Constructor
     * @note The name is set to "unnamed_cell_array".
     */
    CellArray();

    /**
     * @brief Constructor
     * @param name The name of the CellArray
     */
    CellArray(const std::string& name);

    /**
     * @brief Constructor
     * @param name The name of the CellArray
     * @param dimensions The dimensions of the CellArray
     */
    CellArray(const std::string& name, const std::vector<index_type>& dimensions);

    /**
     * @brief Constructor
     * @param name The name of the CellArray
     * @param dimensions The dimensions of the CellArray
     * @param elements The elements to be added to the CellArray (they are copied)
     * @note The name of the variables in the vector is not used
     * @note The vector is supposed to contain the variables in column-major format. For example, an array of dimension 2x3x4
     * needs a dimension vector of size 3 with content {2,3,4}. The elements vector is supposed to be 24 dimensional.
     */
    CellArray(const std::string& name, const std::vector<index_type>& dimensions, std::vector<Variable> &elements);

    /**
     * @brief Copy constructor
     */
    CellArray(const CellArray& other);

    /**
     * @brief Move constructor
     */
    CellArray(CellArray&& other);

    /**
     * @brief Constructor to share the data ownership of another variable.
     * @param handler The MatvarHandler handler to the matvar_t which has to be shared.
     */
    CellArray(const MatvarHandler& handler);

    /**
    * Destructor.
    */
    ~CellArray();

    /**
     * @brief Assignement operator (copy) from another CellArray.
     * @param other The other CellArray.
     * @note Also the name is copied
     * @return A reference to this CellArray.
     */
    CellArray& operator=(const CellArray& other);

    /**
     * @brief Assignement operator (move) from another CellArray.
     * @param other The other CellArray.
     * @note Also the name is copied
     * @return A reference to this CellArray.
     */
    CellArray& operator=(CellArray&& other);

    /**
     * @brief Set from a vector of Variables
     * @note The vector is supposed to contain the variables in column-major format.
     * @note The name of the variables is not used
     * @param dimensions The input dimensions
     * @param elements The input vector of variables.
     * @return True if successful. False otherwise, for example if one of the input variables is not valid.
     */
    bool fromVectorOfVariables(const std::vector<index_type>& dimensions, std::vector<Variable> &elements);

    /**
     * @brief Get the linear index corresponding to the provided indices
     * @param el The desired element
     * @warning It checks if the element is in the bounds only in debug mode.
     *
     * Since the variables are stored internally in column-major, an element (i,j,k,l,..) of an array
     * of dimensions (n,m,p,k,...) corresponds to a row index
     * equal to i + j*n + k*n*m + l*n*m*p +...
     *
     * @return the linear index corresponding to the provided indices
     */
    index_type rawIndexFromIndices(const std::vector<index_type>& el) const;

    /**
     * @brief Get the indices given the raw index
     * @paragraph rawIndex The input raw index from which to compute the indices
     * @param el The output indices

    * @return True if successful, false otherwise (for example if rawIndex is out of bounds)
     */
    bool indicesFromRawIndex(size_t rawIndex, std::vector<index_type>& el) const;

    /**
     * @brief Change the name of the Variable
     * @param newName The new name
     * @return True if successful.
     */
    bool setName(const std::string& newName);

    /**
     * @brief Resize the cell array.
     * @param newDimensions The new dimensions.
     *
     * @warning Previous data is lost.
     */
    void resize(const std::vector<index_type>& newDimensions);

    /**
     * @brief Get the total number of elements in the array
     * @return The total number of elements
     */
    index_type numberOfElements() const;

    /**
     * @brief Set the element at the specified position
     * @param el The indices of the specified element
     * @param newValue The Variable that will be copied in the specified location
     * @return True if successful, false otherwise (for example if the newValue is not valid)
     * @note An assertion is thrown if el is out of bounds, but only in debug mode
     */
    bool setElement(const std::vector<index_type>& el, const Variable& newValue);

    /**
     * @brief Set the element at the specified position
     * @param el The raw index of the specified element
     * @param newValue The Variable that will be copied in the specified location
     * @return True if successful, false otherwise (for example if the newValue is not valid)
     * @note An assertion is thrown if el is out of bounds, but only in debug mode
     */
    bool setElement(index_type el, const Variable& newValue);

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A Variable with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the variable cannot be resized and the name cannot change.
     */
    matioCpp::Variable operator()(const std::vector<index_type>& el);

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A const Variable with a weak ownership to the underlying mat variable.
     */
    const matioCpp::Variable operator()(const std::vector<index_type>& el) const;

    /**
     * @brief Access specified element.
     * @param el The element to be accessed (raw index).
     * @return A Variable with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the variable cannot be resized and the name cannot change.
     */
    matioCpp::Variable operator()(index_type el);

    /**
     * @brief Access specified element.
     * @param el The element to be accessed (raw index).
     * @return A const Variable with a weak ownership to the underlying mat variable.
     */
    const matioCpp::Variable operator()(index_type el) const;

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A Variable with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the variable cannot be resized and the name cannot change.
     */
    matioCpp::Variable operator[](const std::vector<index_type>& el);

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A const Variable with a weak ownership to the underlying mat variable.
     */
    const matioCpp::Variable operator[](const std::vector<index_type>& el) const;

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @return A Variable with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the variable cannot be resized and the name cannot change.
     */
    matioCpp::Variable operator[](index_type el);

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @return A const Variable with a weak ownership to the underlying mat variable.
     */
    const matioCpp::Variable operator[](index_type el) const;
};

#endif // MATIOCPP_CELLARRAY_H
