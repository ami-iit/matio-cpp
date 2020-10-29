#ifndef MATIOCPP_MULTIDIMENSIONALARRAY_TPP
#define MATIOCPP_MULTIDIMENSIONALARRAY_TPP

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

template<typename T>
bool matioCpp::MultiDimensionalArray<T>::checkCompatibility(const matvar_t* inputPtr, matioCpp::VariableType variableType, matioCpp::ValueType valueType) const
{

    if ((variableType != matioCpp::VariableType::MultiDimensionalArray) &&
        (variableType != matioCpp::VariableType::Vector) &&
        (variableType != matioCpp::VariableType::Element))
    {
        std::cerr << "[matioCpp::MultiDimensionalArray::checkCompatibility] The variable type is not compatible with a multidimensional array." << std::endl;
        return false;
    }

    if (inputPtr->isComplex)
    {
        std::cerr << "[matioCpp::MultiDimensionalArray::checkCompatibility] Cannot use a complex variable into a non-complex one." << std::endl;
        return false;
    }

    if (!matioCpp::is_convertible_to_primitive_type<T>(valueType))
    {
        std::string dataType = "";
        std::string classType = "";

        get_types_names_from_matvart(inputPtr, classType, dataType);

        std::cerr << "[matioCpp::MultiDimensionalArray::checkCompatibility] The value type is not convertible to " <<
            typeid(T).name() <<"." << std::endl <<
            "                                                      Input class type: " << classType << std::endl <<
            "                                                      Input data type: " << dataType << std::endl;
        return false;
    }
    return true;
}

template<typename T>
matioCpp::MultiDimensionalArray<T>::MultiDimensionalArray()
{
    std::vector<T> empty;
    constexpr size_t emptyDimensions[] = {0, 0, 0};
    initializeVariable("unnamed_multidimensional_array",
                       VariableType::MultiDimensionalArray,
                       matioCpp::get_type<T>::valueType(), emptyDimensions,
                       (void*)empty.data());
}

template<typename T>
matioCpp::MultiDimensionalArray<T>::MultiDimensionalArray(const std::string &name)
{
    std::vector<T> empty;
    constexpr size_t emptyDimensions[] = {0, 0, 0};
    initializeVariable(name,
                       VariableType::MultiDimensionalArray,
                       matioCpp::get_type<T>::valueType(), emptyDimensions,
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
                       matioCpp::get_type<T>::valueType(), dimensions,
                       (void*)dummy.data());
}

template<typename T>
matioCpp::MultiDimensionalArray<T>::MultiDimensionalArray(const std::string &name, const std::vector<matioCpp::MultiDimensionalArray<T>::index_type> &dimensions, matioCpp::MultiDimensionalArray<T>::const_pointer inputVector)
{
    for (matioCpp::MultiDimensionalArray<T>::index_type dim : dimensions)
    {
        if (dim == 0)
        {
            std::cerr << "[ERROR][matioCpp::MultiDimensionalArray::MultiDimensionalArray] Zero dimension detected." << std::endl;
            assert(false);
        }
    }

    initializeVariable(name,
                       VariableType::MultiDimensionalArray,
                       matioCpp::get_type<T>::valueType(), dimensions,
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
    fromOther(std::forward<matioCpp::MultiDimensionalArray<T>>(other));
}

template<typename T>
matioCpp::MultiDimensionalArray<T>::MultiDimensionalArray(const MatvarHandler &handler)
    : matioCpp::Variable(handler)
{
    if (!handler.get() || !checkCompatibility(handler.get(), handler.variableType(), handler.valueType()))
    {
        assert(false);
        std::vector<T> empty;
        constexpr size_t emptyDimensions[] = {0, 0, 0};
        initializeVariable("unnamed_multidimensional_array",
                           VariableType::MultiDimensionalArray,
                           matioCpp::get_type<T>::valueType(), emptyDimensions,
                           (void*)empty.data());
    }
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
    fromOther(std::forward<matioCpp::MultiDimensionalArray<T>>(other));
    return *this;
}

template<typename T>
bool matioCpp::MultiDimensionalArray<T>::fromVectorizedArray(const std::vector<typename matioCpp::MultiDimensionalArray<T>::index_type> &dimensions, matioCpp::MultiDimensionalArray<T>::const_pointer inputVector)
{
    for (matioCpp::MultiDimensionalArray<T>::index_type dim : dimensions)
    {
        if (dim == 0)
        {
            std::cerr << "[ERROR][matioCpp::MultiDimensionalArray::fromVectorizedArray] Zero dimension detected." << std::endl;
            return false;
        }
    }

    return initializeVariable(name(),
                              VariableType::MultiDimensionalArray,
                              matioCpp::get_type<T>::valueType(), dimensions,
                              (void*)inputVector);
}

template<typename T>
typename matioCpp::MultiDimensionalArray<T>::index_type matioCpp::MultiDimensionalArray<T>::rawIndexFromIndices(const std::vector<typename matioCpp::MultiDimensionalArray<T>::index_type> &el) const
{
    assert(dimensions().size() > 0 && numberOfElements() > 0 && "[matioCpp::MultiDimensionalArray::rawIndexFromIndices] The array is empty.");
    assert(el.size() > 0 == dimensions().size() > 0 && "[matioCpp::MultiDimensionalArray::rawIndexFromIndices] The input vector el should have the same number of dimensions of the array.");
    assert(el[0] < dimensions()[0] && "[matioCpp::MultiDimensionalArray::rawIndexFromIndices] The required element is out of bounds.");

    typename matioCpp::MultiDimensionalArray<T>::index_type index = 0;
    typename matioCpp::MultiDimensionalArray<T>::index_type previousDimensionsFactorial = 1;

    for (size_t i = 0; i < el.size(); ++i)
    {
        assert(el[i] < dimensions()[i] && "[matioCpp::MultiDimensionalArray::rawIndexFromIndices] The required element is out of bounds.");
        index += el[i] * previousDimensionsFactorial;
        previousDimensionsFactorial *= dimensions()[i];
    }

    return index;
}

template<typename T>
bool matioCpp::MultiDimensionalArray<T>::indicesFromRawIndex(size_t rawIndex, std::vector<typename matioCpp::MultiDimensionalArray<T>::index_type> &el) const
{
    el.resize(dimensions().size());

    if (rawIndex >= numberOfElements())
    {
        std::cerr << "[ERROR][matioCpp::MultiDimensionalArray::indicesFromRawIndex] rawIndex is greater than the number of elements." << std::endl;
        return false;
    }

    typename matioCpp::MultiDimensionalArray<T>::index_type previousDimensionsFactorial = dimensions()[0];

    //First we fill el with the factorial of the dimensions

    for (size_t i = 1; i < el.size(); ++i)
    {
        el[i - 1] = previousDimensionsFactorial;
        previousDimensionsFactorial *= dimensions()[i];
    }

    typename matioCpp::MultiDimensionalArray<T>::index_type remainder = rawIndex;

    for (size_t i = el.size() - 1; i > 0; --i)
    {
        el[i] = remainder / el[i - 1];
        remainder -= el[i] * el[i - 1];
    }
    el[0] = remainder;

    return true;
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
    return changeName(newName);
}

template<typename T>
void matioCpp::MultiDimensionalArray<T>::resize(const std::vector<typename matioCpp::MultiDimensionalArray<T>::index_type> &newDimensions)
{
    matioCpp::MultiDimensionalArray<T>::index_type totalElements = 1;
    for (matioCpp::MultiDimensionalArray<T>::index_type dim : newDimensions)
    {
        if (dim == 0)
        {
            std::cerr << "[ERROR][matioCpp::MultiDimensionalArray::resize] Zero dimension detected." << std::endl;
            assert(false);
        }

        totalElements *= dim;
    }

    std::vector<T> dummy(totalElements);

    initializeVariable(name(),
                       VariableType::MultiDimensionalArray,
                       matioCpp::get_type<T>::valueType(), newDimensions,
                       dummy.data());
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
    return getArrayNumberOfElements();
}

template<typename T>
typename matioCpp::MultiDimensionalArray<T>::reference matioCpp::MultiDimensionalArray<T>::operator()(const std::vector<typename matioCpp::MultiDimensionalArray<T>::index_type> &el)
{
    return data()[rawIndexFromIndices(el)];
}

template<typename T>
typename matioCpp::MultiDimensionalArray<T>::value_type matioCpp::MultiDimensionalArray<T>::operator()(const std::vector<typename matioCpp::MultiDimensionalArray<T>::index_type> &el) const
{
    return data()[rawIndexFromIndices(el)];
}

template<typename T>
typename matioCpp::MultiDimensionalArray<T>::reference matioCpp::MultiDimensionalArray<T>::operator()(typename matioCpp::MultiDimensionalArray<T>::index_type el)
{
    assert(el < numberOfElements() && "[matioCpp::MultiDimensionalArray::operator()] The required element is out of bounds.");
    return data()[el];
}

template<typename T>
typename matioCpp::MultiDimensionalArray<T>::value_type matioCpp::MultiDimensionalArray<T>::operator()(typename matioCpp::MultiDimensionalArray<T>::index_type el) const
{
    assert(el < numberOfElements() && "[matioCpp::MultiDimensionalArray::operator()] The required element is out of bounds.");
    return data()[el];
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

template<typename T>
typename matioCpp::MultiDimensionalArray<T>::reference matioCpp::MultiDimensionalArray<T>::operator[](typename matioCpp::MultiDimensionalArray<T>::index_type el)
{
    assert(el < numberOfElements() && "[matioCpp::MultiDimensionalArray::operator[]] The required element is out of bounds.");
    return data()[el];
}

template<typename T>
typename matioCpp::MultiDimensionalArray<T>::value_type matioCpp::MultiDimensionalArray<T>::operator[](typename matioCpp::MultiDimensionalArray<T>::index_type el) const
{
    assert(el < numberOfElements() && "[matioCpp::MultiDimensionalArray::operator[]] The required element is out of bounds.");
    return data()[el];
}

template<typename T>
matioCpp::MultiDimensionalArray<T> matioCpp::Variable::asMultiDimensionalArray()
{
    return matioCpp::MultiDimensionalArray<T>(*m_handler);
}

template<typename T>
const matioCpp::MultiDimensionalArray<T> matioCpp::Variable::asMultiDimensionalArray() const
{
    return matioCpp::MultiDimensionalArray<T>(*m_handler);
}

#endif // MATIOCPP_MULTIDIMENSIONALARRAY_TPP
