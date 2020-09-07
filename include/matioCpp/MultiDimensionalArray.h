#ifndef MATIOCPP_MULTIDIMENSIONALARRAY_H
#define MATIOCPP_MULTIDIMENSIONALARRAY_H

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
 * @brief MultiDimensionalArray is a particular type of Variable specialized for multidimensional arrays of a generic type T.
 * @note The underlying array is in column-major format.
 */
template<typename T>
class matioCpp::MultiDimensionalArray : public matioCpp::Variable
{
public:

    using element_type = T; /** Defines the type of an element of the Vector. Needed to use the iterator. **/

    using value_type = std::remove_cv_t<T>; /** Defines the type of an element of the Vector without "const". Useful to use make_span. **/

    using allocator_type = std::allocator<T>; /** Defines how to allocate T. **/

    using index_type = size_t; /** The type used for indices. **/

    using reference = element_type&; /** The reference type. **/

    using pointer = typename std::allocator_traits<std::allocator<T>>::pointer; /** The pointer type. **/

    using const_pointer = typename std::allocator_traits<std::allocator<T>>::const_pointer; /** The const pointer type. **/

    /**
     * @brief Default Constructor
     * @note The name is set to "unnamed_multidimensional_array".
     */
    MultiDimensionalArray();

    /**
     * @brief Constructor
     * @param name The name of the MultiDimensionalArray
     */
    MultiDimensionalArray(const std::string& name);

    /**
     * @brief Constructor
     * @param name The name of the MultiDimensionalArray
     * @param dimensions The dimensions of the MultiDimensionalArray
     */
    MultiDimensionalArray(const std::string& name, const std::vector<index_type>& dimensions);

    /**
     * @brief Constructor
     * @param name The name of the MultiDimensionalArray
     * @param dimensions The dimensions of the MultiDimensionalArray
     * @param inputVector The raw pointer to the data stored in column-major order
     */
    MultiDimensionalArray(const std::string& name, const std::vector<index_type>& dimensions, const_pointer inputVector);

    /**
     * @brief Copy constructor
     */
    MultiDimensionalArray(const MultiDimensionalArray<T>& other);

    /**
     * @brief Move constructor
     */
    MultiDimensionalArray(MultiDimensionalArray<T>&& other);

    /**
    * Destructor.
    */
    ~MultiDimensionalArray();

    /**
     * @brief Assignement operator (copy) from another MultiDimensionalArray.
     * @param other The other MultiDimensionalArray.
     * @note Also the name is copied
     * @return A reference to this MultiDimensionalArray.
     */
    MultiDimensionalArray<T>& operator=(const MultiDimensionalArray<T>& other);

    /**
     * @brief Assignement operator (move) from another MultiDimensionalArray.
     * @param other The other MultiDimensionalArray.
     * @note Also the name is copied
     * @return A reference to this MultiDimensionalArray.
     */
    MultiDimensionalArray<T>& operator=(MultiDimensionalArray<T>&& other);

    /**
     * @brief Set from a vectorized array
     * @note The pointer is supposed to be in column-major format.
     * @param dimensions The input dimensions
     * @param inputVector The input pointer.
     * @return True if successfull.
     */
    bool fromVectorizedArray(const std::vector<index_type>& dimensions, const_pointer inputVector);

    /**
     * @brief Get the index in the vectorized array corresponding to the provided indices
     * @param el The desider element
     * @warning It checks if the element is in the bounds only in debug mode.
     * @return the index in the vectorized array corresponding to the provided indices
     */
    index_type rawIndexFromIndices(const std::vector<index_type>& el) const;

    /**
     * Inherited from matioCpp::Variable
     */
    virtual bool fromOther(const Variable& other) final;

    /**
     * Inherited from matioCpp::Variable
     */
    virtual bool fromOther(Variable&& other) final;

    /**
     * Inherited from matioCpp::Variable
     */
    virtual bool fromMatio(const matvar_t * inputVar) final;

    /**
     * @brief Get this MultiDimensionalArray as a Span
     */
    matioCpp::Span<T> toSpan();

    /**
     * @brief Get this MultiDimensionalArray as a Span (const version)
     */
    const matioCpp::Span<const T> toSpan() const;

    /**
     * @brief Change the name of the Variable
     * @param newName The new name
     * @return True if successfull.
     *
     * @warning This requires the MultiDimensionalArray to be reallocated. It performs memory allocation.
     */
    bool setName(const std::string& newName);

    /**
     * @brief Resize the vector.
     * @param newSize The new size.
     *
     * @warning This requires to allocate memory for twice the new size.
     * @warning Previous data is lost.
     */
    void resize(const std::vector<index_type>& newDimensions);

    /**
     * @brief Direct access to the underlying array.
     * @note The underlying array is in column-major format.
     * @return A pointer to the internal data.
     */
    pointer data();

    /**
     * @brief Direct access to the underlying array.
     * @note The underlying array is in column-major format.
     * @return A pointer to the internal data.
     */
    const_pointer data() const;

    /**
     * @brief Get the total number of elements in the array
     * @return The total number of elements
     */
    index_type numberOfElements() const;

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A reference to the element.
     */
    reference operator()(const std::vector<index_type>& el);

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A copy to the element.
     */
    value_type operator()(const std::vector<index_type>& el) const;

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A reference to the element.
     */
    reference operator[](const std::vector<index_type>& el);

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A copy to the element.
     */
    value_type operator[](const std::vector<index_type>& el) const;
};


/////////----------------------Implementation

template<typename T>
matioCpp::MultiDimensionalArray<T>::MultiDimensionalArray()
{
    std::vector<T> empty;
    initializeVariable("unnamed_multidimensional_array",
                       VariableType::MultiDimensionalArray,
                       matioCpp::get_type<T>::valueType, {0, 0, 0},
                       (void*)empty.data());
}

template<typename T>
matioCpp::MultiDimensionalArray<T>::MultiDimensionalArray(const std::string &name)
{
    std::vector<T> empty;
    initializeVariable(name,
                       VariableType::MultiDimensionalArray,
                       matioCpp::get_type<T>::valueType, {0, 0, 0},
                       (void*)empty.data());
}

template<typename T>
matioCpp::MultiDimensionalArray<T>::MultiDimensionalArray(const std::string &name, const std::vector<matioCpp::MultiDimensionalArray<T>::index_type> &dimensions)
{
    matioCpp::MultiDimensionalArray<T>::index_type totalElements = 1;
    for (matioCpp::MultiDimensionalArray<T>::index_type dim : dimensions)
    {
        if (dim == 0)
        {
            std::cerr << "[ERROR][matioCpp::MultiDimensionalArray::MultiDimensionalArray] Zero dimension detected." << std::endl;
            assert(false);
        }

        totalElements *= dim;
    }

    std::vector<T> dummy(totalElements);

    initializeVariable(name,
                       VariableType::MultiDimensionalArray,
                       matioCpp::get_type<T>::valueType, dimensions,
                       (void*)dummy.data());
}

template<typename T>
matioCpp::MultiDimensionalArray<T>::MultiDimensionalArray(const std::string &name, const std::vector<matioCpp::MultiDimensionalArray<T>::index_type> &dimensions, matioCpp::MultiDimensionalArray<T>::const_pointer inputVector)
{
    matioCpp::MultiDimensionalArray<T>::index_type totalElements = 1;
    for (matioCpp::MultiDimensionalArray<T>::index_type dim : dimensions)
    {
        if (dim == 0)
        {
            std::cerr << "[ERROR][matioCpp::MultiDimensionalArray::MultiDimensionalArray] Zero dimension detected." << std::endl;
            assert(false);
        }

        totalElements *= dim;
    }

    initializeVariable(name,
                       VariableType::MultiDimensionalArray,
                       matioCpp::get_type<T>::valueType, dimensions,
                       (void*)inputVector);
}

template<typename T>
matioCpp::MultiDimensionalArray<T>::MultiDimensionalArray(const MultiDimensionalArray<T> &other)
{
    fromOther(other);
}

template<typename T>
matioCpp::MultiDimensionalArray<T>::MultiDimensionalArray(MultiDimensionalArray<T> &&other)
{
    fromOther(other);
}

template<typename T>
matioCpp::MultiDimensionalArray<T>::~MultiDimensionalArray()
{

}

template<typename T>
matioCpp::MultiDimensionalArray<T> &matioCpp::MultiDimensionalArray<T>::operator=(const matioCpp::MultiDimensionalArray<T> &other)
{
    fromOther(other);
    return *this;
}

template<typename T>
matioCpp::MultiDimensionalArray<T> &matioCpp::MultiDimensionalArray<T>::operator=(matioCpp::MultiDimensionalArray<T> &&other)
{
    fromOther(other);
    return *this;
}

template<typename T>
bool matioCpp::MultiDimensionalArray<T>::fromVectorizedArray(const std::vector<typename matioCpp::MultiDimensionalArray<T>::index_type> &dimensions, matioCpp::MultiDimensionalArray<T>::const_pointer inputVector)
{
    matioCpp::MultiDimensionalArray<T>::index_type totalElements = 1;
    for (matioCpp::MultiDimensionalArray<T>::index_type dim : dimensions)
    {
        if (dim == 0)
        {
            std::cerr << "[ERROR][matioCpp::MultiDimensionalArray::fromVectorizedArray] Zero dimension detected." << std::endl;
            return false;
        }

        totalElements *= dim;
    }

    return initializeVariable(name(),
                              VariableType::MultiDimensionalArray,
                              matioCpp::get_type<T>::valueType, dimensions,
                              (void*)inputVector);
}

template<typename T>
typename matioCpp::MultiDimensionalArray<T>::index_type matioCpp::MultiDimensionalArray<T>::rawIndexFromIndices(const std::vector<typename matioCpp::MultiDimensionalArray<T>::index_type> &el) const
{
    assert(dimensions().size() && numberOfElements() && "[matioCpp::MultiDimensionalArray::rawIndexFromIndices] The array is empty.");
    assert(el.size() == dimensions().size() && "[matioCpp::MultiDimensionalArray::rawIndexFromIndices] The input vector el should have the same number of dimensions of the array.");
    assert(el[0] < dimensions()[0] && "[matioCpp::MultiDimensionalArray::operator()] The required element is out of bounds.");

    typename matioCpp::MultiDimensionalArray<T>::index_type index = 0;
    typename matioCpp::MultiDimensionalArray<T>::index_type previousDimensionsFactorial = 1;

    for (size_t i = 0; i < el.size(); ++i)
    {
        assert(el[i] < dimensions()[i] && "[matioCpp::MultiDimensionalArray::operator()] The required element is out of bounds.");
        index += el[i] * previousDimensionsFactorial;
        previousDimensionsFactorial *= dimensions()[i];
    }

    return index;
}

template<typename T>
bool matioCpp::MultiDimensionalArray<T>::fromOther(const matioCpp::Variable &other)
{
    if (other.variableType() != matioCpp::VariableType::MultiDimensionalArray)
    {
        std::cerr << "[matioCpp::MultiDimensionalArray::fromOther] The input variable is not a vector." << std::endl;
        return false;
    }

    if (other.isComplex())
    {
        std::cerr << "[matioCpp::MultiDimensionalArray::fromOther] The input variable is complex, this is not." << std::endl;
        return false;
    }

    if (!matioCpp::is_convertible_to_primitive_type<T>(other.valueType()))
    {
        std::cerr << "[matioCpp::MultiDimensionalArray::fromOther] The input type is not convertible to " <<
            typeid(T).name() <<"." << std::endl;
        return false;
    }
    return Variable::fromOther(other);
}

template<typename T>
bool matioCpp::MultiDimensionalArray<T>::fromOther(matioCpp::Variable &&other)
{
    if (other.variableType() != matioCpp::VariableType::MultiDimensionalArray)
    {
        std::cerr << "[matioCpp::MultiDimensionalArray::fromOther] The input variable is not a vector." << std::endl;
        return false;
    }

    if (other.isComplex())
    {
        std::cerr << "[matioCpp::MultiDimensionalArray::fromOther] The input variable is complex, this is not." << std::endl;
        return false;
    }

    if (!matioCpp::is_convertible_to_primitive_type<T>(other.valueType()))
    {
        std::cerr << "[matioCpp::MultiDimensionalArray::fromOther] The input type is not convertible to " <<
            typeid(T).name() <<"." << std::endl;
        return false;
    }
    return Variable::fromOther(other);
}

template<typename T>
bool matioCpp::MultiDimensionalArray<T>::fromMatio(const matvar_t *inputVar)
{
    Variable dummy;
    if (!dummy.fromMatio(inputVar))
    {
        return false;
    }

    return fromOther(dummy);
}

template<typename T>
matioCpp::Span<T> matioCpp::MultiDimensionalArray<T>::toSpan()
{
    return matioCpp::make_span(data(), numberOfElements());
}

template<typename T>
const matioCpp::Span<const T> matioCpp::MultiDimensionalArray<T>::toSpan() const
{
    return matioCpp::make_span(data(), numberOfElements());
}

template<typename T>
bool matioCpp::MultiDimensionalArray<T>::setName(const std::string &newName)
{
    return initializeVariable(newName,
                              VariableType::MultiDimensionalArray,
                              matioCpp::get_type<T>::valueType, dimensions(),
                              (void*)data());
}

template<typename T>
void matioCpp::MultiDimensionalArray<T>::resize(const std::vector<typename matioCpp::MultiDimensionalArray<T>::index_type> &newDimensions)
{
    matioCpp::MultiDimensionalArray<T> newArray(name(), newDimensions);
    fromOther(std::move(newArray));
}

template<typename T>
typename matioCpp::MultiDimensionalArray<T>::pointer matioCpp::MultiDimensionalArray<T>::data()
{
    return static_cast<typename matioCpp::MultiDimensionalArray<T>::pointer>(toMatio()->data);
}

template<typename T>
typename matioCpp::MultiDimensionalArray<T>::const_pointer matioCpp::MultiDimensionalArray<T>::data() const
{
    return static_cast<typename matioCpp::MultiDimensionalArray<T>::const_pointer>(toMatio()->data);
}

template<typename T>
typename matioCpp::MultiDimensionalArray<T>::index_type matioCpp::MultiDimensionalArray<T>::numberOfElements() const
{
    matioCpp::MultiDimensionalArray<T>::index_type totalElements = 1;
    for (matioCpp::MultiDimensionalArray<T>::index_type dim : dimensions())
    {
        totalElements *= dim;
    }

    return totalElements;
}

template<typename T>
typename matioCpp::MultiDimensionalArray<T>::reference matioCpp::MultiDimensionalArray<T>::operator()(const std::vector<typename matioCpp::MultiDimensionalArray<T>::index_type> &el)
{
    return data()[rawIndexFromIndices(el)];
}

template<typename T>
typename matioCpp::MultiDimensionalArray<T>::value_type matioCpp::MultiDimensionalArray<T>::operator()(const std::vector<matioCpp::MultiDimensionalArray<T>::index_type> &el) const
{
    return data()[rawIndexFromIndices(el)];
}

template<typename T>
typename matioCpp::MultiDimensionalArray<T>::reference matioCpp::MultiDimensionalArray<T>::operator[](const std::vector<matioCpp::MultiDimensionalArray<T>::index_type> &el)
{
    return data()[rawIndexFromIndices(el)];
}

template<typename T>
typename matioCpp::MultiDimensionalArray<T>::value_type matioCpp::MultiDimensionalArray<T>::operator[](const std::vector<matioCpp::MultiDimensionalArray<T>::index_type> &el) const
{
    return data()[rawIndexFromIndices(el)];
}


#endif // MATIOCPP_MULTIDIMENSIONALARRAY_H

