#ifndef MATIOCPP_CELLARRAY_H
#define MATIOCPP_CELLARRAY_H

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
#include <matioCpp/Span.h>
#include <matioCpp/Variable.h>

/**
 * @brief CellArray is a particular type of Variable specialized for cell arrays.
 */
class matioCpp::CellArray : public matioCpp::Variable
{

    /**
     * @brief Check if an input matio pointer is compatible with the CellArray class.
     * @param inputPtr The input matvar_t pointer.
     * @return True if compatible. False otherwise, throwing errors.
     */
    virtual bool checkCompatibility(const matvar_t* inputPtr) const final;

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
     * @note The name of the variables is not used
     * @note The vector is supposed to contain the variables in column-major format.
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
     * @param inputVector The input pointer.
     * @return True if successfull.
     */
    bool fromVectorOfVariables(const std::vector<index_type>& dimensions, std::vector<Variable> &elements);

    /**
     * @brief Get the linear index corresponding to the provided indices
     * @param el The desider element
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
     * @brief Change the name of the Variable
     * @param newName The new name
     * @return True if successfull.
     *
     * @warning This requires the CellArray to be reallocated. It performs memory allocation.
     */
    bool setName(const std::string& newName);

    /**
     * @brief Resize the vector.
     * @param newDimensions The new dimensions.
     *
     * @warning This requires to allocate memory for twice the new size.
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
     * @return True if successfull, false otherwise (for example if the newValue is not valid)
     * @note An assertion is thrown if el is out of bounds, but only in debug mode
     */
    bool setElement(const std::vector<index_type>& el, const Variable& newValue);

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
};

#endif // MATIOCPP_CELLARRAY_H
