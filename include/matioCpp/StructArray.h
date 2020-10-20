#ifndef MATIOCPP_STRUCTARRAY_H
#define MATIOCPP_STRUCTARRAY_H

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
#include <matioCpp/StructArrayElement.h>

/**
 * @brief StructArray is a particular type of Variable specialized for array of structs.
 */
class matioCpp::StructArray : public matioCpp::Variable
{

    /**
     * @brief Check if an input matio pointer is compatible with the StructArray class.
     * @param inputPtr The input matvar_t pointer.
     * @return True if compatible. False otherwise, throwing errors.
     */
    virtual bool checkCompatibility(const matvar_t* inputPtr) const final;

    template<bool>
    friend class StructArrayElement;

public:

    using index_type = size_t; /** The type used for indices. **/

    using Element = StructArrayElement<false>; /** Non-const version of StructArrayElement. **/

    using ConstElement = StructArrayElement<true>; /** Const version of Element. **/

    /**
     * @brief Default Constructor
     * @note The name is set to "unnamed_struct_array".
     */
    StructArray();

    /**
     * @brief Constructor
     * @param name The name of the StructArray
     */
    StructArray(const std::string& name);

    /**
     * @brief Constructor
     * @param name The name of the StructArray
     * @param dimensions The dimensions of the StructArray
     */
    StructArray(const std::string& name, const std::vector<index_type>& dimensions);

    /**
     * @brief Constructor
     * @param name The name of the StructArray
     * @param dimensions The dimensions of the StructArray
     * @param elements The elements to be added to the StructArray (they are copied)
     * @note The name of the variables is not used
     * @note The vector is supposed to contain the variables in column-major format.
     * @note Each element is supposed to have the same set fields.
     */
    StructArray(const std::string& name,
                const std::vector<index_type>& dimensions,
                const std::vector<matioCpp::Struct>& elements);

    /**
     * @brief Copy constructor
     */
    StructArray(const StructArray& other);

    /**
     * @brief Move constructor
     */
    StructArray(StructArray&& other);

    /**
     * @brief Constructor to share the data ownership of another variable.
     * @param handler The MatvarHandler handler to the matvar_t which has to be shared.
     */
    StructArray(const MatvarHandler& handler);

    /**
    * Destructor.
    */
    ~StructArray();

    /**
     * @brief Assignement operator (copy) from another StructArray.
     * @param other The other StructArray.
     * @note Also the name is copied
     * @return A reference to this StructArray.
     */
    StructArray& operator=(const StructArray& other);

    /**
     * @brief Assignement operator (move) from another StructArray.
     * @param other The other StructArray.
     * @note Also the name is copied
     * @return A reference to this StructArray.
     */
    StructArray& operator=(StructArray&& other);

    /**
     * @brief Set from a vector of Variables
     * @param dimensions The input dimensions
     * @param elements The elements to be set.
     * @note Previous structs are lost
     * @note Each struct is supposed to have the same set of fields
     * @return True if successful. False otherwise, for example if one of the input variables is not valid.
     */
    bool fromVectorOfStructs(const std::vector<index_type> &dimensions, const std::vector<matioCpp::Struct>& elements);

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
     *
     * @warning This requires the Struct to be reallocated. It performs memory allocation.
     */
    bool setName(const std::string& newName);

    /**
     * @brief Resize the array.
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
     * @brief Add a new field to all the structs
     * @param newField The new field
     * @return True on success, false otherwise
     */
    bool addField(const std::string& newField);

    /**
     * @brief Set the element at the specified position
     * @param el The indices of the specified elementconst StructArrayElement
     * @param newValue The Struct that will be copied in the specified location
     * @return True if successful, false otherwise (for example if the newValue is not valid)
     * @note An assertion is thrown if el is out of bounds, but only in debug mode
     * @note The struct needs to have the same set of fields returned by fields()
     */
    bool setElement(const std::vector<index_type>& el, const matioCpp::Struct& newValue);

    /**
     * @brief Set the element at the specified position
     * @param el The raw index of the specified element
     * @param newValue The Struct that will be copied in the specified location
     * @return True if successful, false otherwise (for example if the newValue is not valid)
     * @note An assertion is thrown if el is out of bounds, but only in debug mode
     * @note The struct needs to have the same set of fields returned by fields()
     */
    bool setElement(index_type el, const matioCpp::Struct& newValue);

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A Struct with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the name cannot change, and no fields can be added.
     */
    Element operator()(const std::vector<index_type>& el);

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A const Struct with a weak ownership to the underlying mat variable.
     */
    ConstElement operator()(const std::vector<index_type>& el) const;

    /**
     * @brief Access specified element.
     * @param el The element to be accessed (raw index).
     * @return A Struct with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the name cannot change, and no fields can be added.
     */
    Element operator()(index_type el);

    /**
     * @brief Access specified element.
     * @param el The element to be accessed (raw index).
     * @return A const Struct with a weak ownership to the underlying mat variable.
     */
    ConstElement operator()(index_type el) const;

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A Struct with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the name cannot change, and no fields can be added.
     */
    Element operator[](const std::vector<index_type>& el);

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A const Struct with a weak ownership to the underlying mat variable.
     */
    ConstElement operator[](const std::vector<index_type>& el) const;

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @return A Struct with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the name cannot change, and no fields can be added.
     */
    Element operator[](index_type el);

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @return A const Struct with a weak ownership to the underlying mat variable.
     */
    ConstElement operator[](index_type el) const;
};

#endif // MATIOCPP_STRUCTARRAY_H
