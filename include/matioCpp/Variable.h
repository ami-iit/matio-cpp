#ifndef MATIOCPP_VARIABLE_H
#define MATIOCPP_VARIABLE_H
/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */


#include <matioCpp/ForwardDeclarations.h>
#include <matioCpp/ConversionUtilities.h>
#include <matioCpp/Span.h>
#include <matioCpp/MatvarHandler.h>
#include <matioCpp/SharedMatvar.h>
#include <matioCpp/WeakMatvar.h>


/**
 * @brief The matioCpp::Variable class is the equivalent of matvar_t in matio. It is supposed to be a basic access to object that are or need to be saved in a mat file.
 */
class matioCpp::Variable
{

    matioCpp::MatvarHandler* m_handler;

protected:

    /**
     * @brief Initialize the variable.
     *
     * @note Matlab stores data in column-major format (https://it.mathworks.com/help/coder/ug/what-are-column-major-and-row-major-representation-1.html).
     * Hence, given a multi-dimensional array of size n x m x p, the element (i, j, k) (with zero indexing) is at position i + j*n + k*n*m
     *
     * @param name The name of the variable.
     * @param variableType The type of variable
     * @param valueType The type of each element in the variable
     * @param dimensions Vector containing the variable dimensions. The size of this vector should be at least 2.
     * @param data A void pointer to the (flattened) data.
     * @return true in case the variable was correctly initialized.
     */
    bool initializeVariable(const std::string &name, const VariableType &variableType, const ValueType &valueType, matioCpp::Span<const size_t> dimensions, void *data);

    /**
     * @brief Initialize a complex variable
     * @param name The name of the variable.
     * @param variableType The type of variable
     * @param valueType The type of each element in the variable
     * @param dimensions Vector containing the variable dimensions. The size of this vector should be at least 2.
     * @param realData A void pointer to the (flattened) real data. Check the documentation of initializeVariable for understanding how to obtain/interpret this vector.
     * @param imaginaryData A void pointer to the (flattened) imaginary data. Check the documentation of initializeVariable for understanding how to obtain/interpret this vector.
     * @return true in case the variable was correctly initialized.
     */
    bool initializeComplexVariable(const std::string& name, const VariableType& variableType, const ValueType& valueType, matioCpp::Span<const size_t> dimensions, void *realData, void *imaginaryData);


    /**
     * @brief Initialize a complex vector.
     * @param name The name of the variable.
     * @param realInputVector The real input vector.
     * @param imaginaryInputVector The imaginary input vector.
     * @return true in case the variable was correctly initialized.
     */
    template<typename T>
    bool initializeComplexVector(const std::string& name, const Span<T> realInputVector, const Span<T> imaginaryInputVector)
    {
        if (realInputVector.size() != imaginaryInputVector.size())
        {
            std::string errorPrefix = "[ERROR][matioCpp::Variable::createComplexVector] ";
            if (name.empty())
            {
                std::cerr << errorPrefix << "The real and imaginary part have different size." << std::endl;
                return false;
            }
        }
        return initializeComplexVariable(name, VariableType::Vector, get_type<T>::valuetype, {realInputVector.size(), 1}, (void*)realInputVector.data(), (void*)imaginaryInputVector.data());
    }

    /**
     * @brief Change the name of the variable
     * @param newName The new name to set
     * @return True if successful, false otherwise.
     */
    bool changeName(const std::string& newName);

    /**
     * @brief Get the total number of elements in the array
     * @return The total number of elements
     */
    size_t getArrayNumberOfElements() const;

    /**
     * @brief Set a cell element at a specified linear position
     * @param linearIndex The linear index of the specified element
     * @param newValue The Variable that will be copied in the specified location
     * @return True if successful, false otherwise
     */
    bool setCellElement(size_t linearIndex, const Variable& newValue);

    /**
     * @brief Get a cell element at a specified linear position
     * @param linearIndex The linear index of the specified element
     * @return A Variable with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the variable cannot be resized and the name cannot change.
     */
    Variable getCellElement(size_t linearIndex);

    /**
     * @brief Get a cell element at a specified linear position
     * @param linearIndex The linear index of the specified element
     * @return A const Variable with a weak ownership to the underlying mat variable.
     */
    const Variable getCellElement(size_t linearIndex) const;

    /**
     * @brief Get the total number of fields in the variable, considered as a struct
     * @return The total number of fields
     */
    size_t getStructNumberOfFields() const;

    /**
     * @brief Get the list of fields in the variable, considered as a struct
     * @return The list of fields.
     */
    char * const *getStructFields() const;

    /**
     * @brief Get the index of the specified field in the variable, considered as a struct
     * @param field The field to search
     * @param index The index of the field
     * @return True if successfull, false otherwise.
     */
    bool getStructFieldIndex(const std::string& field, size_t& index) const;

    /**
     * @brief Set the field of the struct at the specified position
     * @param index The linear index of the specified field
     * @param newValue The Variable that will be copied in the specified location
     * @param structPositionInArray The linear position of the struct to set in the struct array
     * @return True if successful, false otherwise (for example if the newValue is not valid)
     */
    bool setStructField(size_t index, const Variable& newValue, size_t structPositionInArray = 0);

    /**
     * @brief Add a new field to the variable, considered as a struct
     * @param newField The new field
     * @return True on success, false otherwise, for example if the struct is part of an array
     */
    bool addStructField(const std::string& newField);

    /**
     * @brief Set the field of the struct given the newValue name
     * @param field The field to set
     * @param newValue The Variable that will be copied in the specified field
     * @param structPositionInArray The linear position of the struct to set in the struct array
     * @return True if successful, false otherwise (for example if the newValue is not valid)
     * @note If the field is not found, a new field is created and appended to the struct,
     *  but only if the struct is not part of an array.
     */
    bool setStructField(const std::string &field, const Variable& newValue, size_t structPositionInArray = 0);

    /**
     * @brief Get the specified field in the variable, considered as a struct
     * @param index The index of the field
     * @param structPositionInArray The linear position of the struct to set in the struct array
     * @return A Variable with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the variable cannot be resized and the name cannot change.
     */
    Variable getStructField(size_t index, size_t structPositionInArray = 0);

    /**
     * @brief Get the specified field in the variable, considered as a struct
     * @param index The index of the field
     * @param structPositionInArray The linear position of the struct to set in the struct array
     * @return A const Variable with a weak ownership to the underlying mat variable.
     */
    const Variable getStructField(size_t index, size_t structPositionInArray = 0) const;

    /**
     * @brief Get an element of the variable, considered as a StructArray
     * @note This allocates memory, one pointer per sruct field, but the pointers point to data in the array
     * @param linearIndex The linear index of the struct to retrieve
     * @return A Struct with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the variable cannot be resized and the name cannot change.
     */
    Struct getStructArrayElement(size_t linearIndex);

    /**
     * @brief Get an element of the variable, considered as a StructArray
     * @note This allocates memory, one pointer per sruct field, but the pointers point to data in the array
     * @param linearIndex The linear index of the struct to retrieve
     * @return A const Struct with a weak ownership to the underlying mat variable.
     */
    const Struct getStructArrayElement(size_t linearIndex) const;

    /**
     * @brief Check if an input matio pointer is compatible with the specified variable.
     * @param inputPtr The input matvar_t pointer.
     * @param variableType The type of variable.
     * @param valueType The value type.
     * @return True if compatible. False otherwise, throwing errors.
     */
    virtual bool checkCompatibility(const matvar_t* inputPtr, matioCpp::VariableType variableType, matioCpp::ValueType valueType) const;

public:

    /**
     * @brief Default constructor
     */
    Variable();

    /**
     * @brief Constructor from an existing matvar_t pointer.
     * @param inputVar The input pointer. It has to be not null.
     */
    Variable(const matvar_t * inputVar);

    /**
     * @brief Copy constructor
     */
    Variable(const Variable& other);

    /**
     * @brief Move constructor
     */
    Variable(Variable&& other);

    /**
     * @brief Constructor to share the data ownership of another variable.
     * @param handler The MatvarHandler handler to the matvar_t which has to be shared.
     */
    Variable(const MatvarHandler& handler);

    /**
    * @brief Destructor
    */
    ~Variable();

    /**
     * @brief Copy assignement
     * @param other The variable to be copied
     * @return A reference to the variable.
     * @note It calls fromOther(), with an assertion on its output.
     */
    Variable& operator=(const Variable& other);

    /**
     * @brief Move assignement
     * @param other The variable to be moved
     * @return A reference to the variable.
     * @note It calls fromOther(), with an assertion on its output.
     */
    Variable& operator=(Variable&& other);

    /**
     * @brief Set this variable from an existing matio variable.
     * @param inputVar The not null pointer to a matio variable. The variable is cloned.
     * @return True if the cloning was successful.
     */
    bool fromMatio(const matvar_t * inputVar);

    /**
     * @brief Set this variable from another variable.
     * @note Child classes need to override this method to make sure that only compatible classes can be copied.
     * @param other The Variable from which to copy the content (data, name, type, dimensions,..).
     * @return True if the cloning was successful.
     */
    bool fromOther(const Variable& other);

    /**
     * @brief Set this variable from another variable.
     * @note No copy is performed, but the original object is no more usable.
     * @note Child classes need to override this method to make sure that only compatible classes can be imported.
     * @param other The source Variable. Content is moved.
     * @return True if the moving was successful.
     */
    bool fromOther(Variable&& other);

    /**
     * @brief Convert this Variable to a matio variable.
     * @warning Any modification to the matio variable is reflected to this Variable.
     * @return A matvar_t pointer.
     */
    const matvar_t * toMatio() const;

    /**
     * @brief Convert this Variable to a matio variable.
     * @warning Any modification to the matio variable is reflected to this Variable.
     * @return A matvar_t pointer.
     */
    matvar_t * toMatio();

    /**
     * @brief Get the name of the Variable.
     * @return The name of the variable.
     */
    std::string name() const;

    /**
     * @brief Get the VariableType.
     * @return The VariableType
     */
    matioCpp::VariableType variableType() const;

    /**
     * @brief Get the ValueType.
     * @return The ValueType
     */
    matioCpp::ValueType valueType() const;

    /**
     * @brief Get if the variable is complex.
     * @return True if complex.
     */
    bool isComplex() const;

    /**
     * @brief Get the dimensions of this object.
     * @return The dimensions of the object
     */
    matioCpp::Span<const size_t> dimensions() const;

    /**
     * @brief Check if the variable is valid
     *
     * A Variable may not be valid if it does not point to any data, or the the dimensions().size() < 2.
     *
     * @return true if valid.
     */
    bool isValid() const;

    /**
     * @brief Access field with specific name.
     * @param el The name of the field to be accessed.
     * @return A Variable with a weak ownership to the underlying mat variable. This means that the data can be changed,
     * but the variable cannot be resized and the name cannot change.
     * @note This works only if the variable is a struct.
     * @note If the field is not found, the output variable is not valid.
     */
    matioCpp::Variable operator[](const std::string& el);

    /**
     * @brief Access field with specific name.
     * @param el The name of the field to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A const Variable with a weak ownership to the underlying mat variable.
     * @note This works only if the variable is a struct.
     * @note If the field is not found, the output variable is not valid.
     */
    const matioCpp::Variable operator[](const std::string& el) const;

    /**
     * @brief Cast the variable as a Element.
     *
     * The implementation is in Element.tpp
     */
    template<typename T>
    matioCpp::Element<T> asElement();

    /**
     * @brief Cast the variable as a const Element
     *
     * The implementation is in Element.tpp
     */
    template<typename T>
    const matioCpp::Element<T> asElement() const;

    /**
     * @brief Cast the variable as a Vector.
     *
     * The implementation is in Vector.tpp
     */
    template<typename T>
    matioCpp::Vector<T> asVector();

    /**
     * @brief Cast the variable as a const Vector
     *
     * The implementation is in Vector.tpp
     */
    template<typename T>
    const matioCpp::Vector<T> asVector() const;

    /**
     * @brief Cast the variable as a String.
     */
    matioCpp::String asString();

    /**
     * @brief Cast the variable as a const String
     */
    const matioCpp::String asString() const;

    /**
     * @brief Cast the variable as a String16.
     */
    matioCpp::String16 asString16();

    /**
     * @brief Cast the variable as a const String16
     */
    const matioCpp::String16 asString16() const;

    /**
     * @brief Cast the variable as a String32.
     */
    matioCpp::String32 asString32();

    /**
     * @brief Cast the variable as a const String32
     */
    const matioCpp::String32 asString32() const;

    /**
     * @brief Cast the variable as a MultiDimensionalArray.
     *
     * The implementation is in MultiDimensionalArray.tpp
     */
    template<typename T>
    matioCpp::MultiDimensionalArray<T> asMultiDimensionalArray();

    /**
     * @brief Cast the variable as a const MultiDimensionalArray.
     *
     * The implementation is in MultiDimensionalArray.tpp
     */
    template<typename T>
    const matioCpp::MultiDimensionalArray<T> asMultiDimensionalArray() const;

    /**
     * @brief Cast the variable as a CellArray.
     */
    matioCpp::CellArray asCellArray();

    /**
     * @brief Cast the variable as a const CellArray.
     */
    const matioCpp::CellArray asCellArray() const;

    /**
     * @brief Cast the variable as a Struct.
     */
    matioCpp::Struct asStruct();

    /**
     * @brief Cast the variable as a const Struct.
     */
    const matioCpp::Struct asStruct() const;

    /**
     * @brief Cast the variable as a StructArray.
     */
    matioCpp::StructArray asStructArray();

    /**
     * @brief Cast the variable as a const StructArray.
     */
    const matioCpp::StructArray asStructArray() const;

};

#endif
