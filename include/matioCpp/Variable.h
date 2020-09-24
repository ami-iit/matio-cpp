#ifndef MATIOCPP_VARIABLE_H
#define MATIOCPP_VARIABLE_H
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
    * @brief Destructor
    */
    ~Variable();

    /**
     * @brief The copy operator has been deleted to avoid a specific child class to be assigned to another one not compatible.
     */
    Variable& operator=(const Variable& other) = delete;

    /**
     * @brief The copy operator has been deleted to avoid a specific child class to be assigned to another one not compatible.
     */
    Variable& operator=(Variable&& other) = delete;

    /**
     * @brief Set this variable from an existing matio variable.
     * @param inputVar The not null pointer to a matio variable. The variable is cloned.
     * @return True if the cloning was successfull.
     */
    virtual bool fromMatio(const matvar_t * inputVar);

    /**
     * @brief Set this variable from another variable.
     * @note Child classes need to override this method to make sure that only compatible classes can be copied.
     * @param other The Variable from which to copy the content (data, name, type, dimensions,..).
     * @return True if the cloning was successful.
     */
    virtual bool fromOther(const Variable& other);

    /**
     * @brief Set this variable from another variable.
     * @note No copy is performed, but the original object is no more usable.
     * @note Child classes need to override this method to make sure that only compatible classes can be imported.
     * @param other The source Variable. Content is moved.
     * @return True if the moving was successful.
     */
    virtual bool fromOther(Variable&& other);

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

};

#endif
