#ifndef MATIOCPP_MULTIDIMENSIONALARRAY_TPP
#define MATIOCPP_MULTIDIMENSIONALARRAY_TPP

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

template<typename T>
bool matioCpp::MultiDimensionalArray<T>::checkCompatibility(const matvar_t *inputPtr) const
{
    if (!inputPtr)
    {
        std::cerr << "[matioCpp::MultiDimensionalArray::isCompatible] The input pointer is null." << std::endl;
        return false;
    }

    matioCpp::VariableType outputVariableType = matioCpp::VariableType::Unsupported;
    matioCpp::ValueType outputValueType = matioCpp::ValueType::UNSUPPORTED;
    get_types_from_matvart(inputPtr, outputVariableType, outputValueType);

    if (outputVariableType != matioCpp::VariableType::MultiDimensionalArray)
    {
        std::cerr << "[matioCpp::MultiDimensionalArray::isCompatible] The input variable is not a multidimensional array." << std::endl;
        return false;
    }

    if (inputPtr->isComplex)
    {
        std::cerr << "[matioCpp::MultiDimensionalArray::isCompatible] Cannot copy a complex variable to a non-complex one." << std::endl;
        return false;
    }

    if (!matioCpp::is_convertible_to_primitive_type<T>(outputValueType))
    {
        std::cerr << "[matioCpp::MultiDimensionalArray::isCompatible] The input type is not convertible to " <<
            typeid(T).name() <<"." << std::endl;
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
                       matioCpp::get_type<T>::valueType, emptyDimensions,
                       (void*)empty.data());
}

template<typename T>
matioCpp::MultiDimensionalArray<T>::MultiDimensionalArray(const std::string &name)
{
    std::vector<T> empty;
    constexpr size_t emptyDimensions[] = {0, 0, 0};
    initializeVariable(name,
                       VariableType::MultiDimensionalArray,
                       matioCpp::get_type<T>::valueType, emptyDimensions,
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
matioCpp::MultiDimensionalArray<T>::MultiDimensionalArray(const MatvarHandler &handler)
    : matioCpp::Variable(handler)
{
    if (!checkCompatibility(handler.get()))
    {
        assert(false);
        std::vector<T> empty;
        constexpr size_t emptyDimensions[] = {0, 0, 0};
        initializeVariable("unnamed_multidimensional_array",
                           VariableType::MultiDimensionalArray,
                           matioCpp::get_type<T>::valueType, emptyDimensions,
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
    fromOther(other);
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
                              matioCpp::get_type<T>::valueType, dimensions,
                              (void*)inputVector);
}

template<typename T>
typename matioCpp::MultiDimensionalArray<T>::index_type matioCpp::MultiDimensionalArray<T>::rawIndexFromIndices(const std::vector<typename matioCpp::MultiDimensionalArray<T>::index_type> &el) const
{
    assert(dimensions().size() > 0 && numberOfElements() > 0 && "[matioCpp::MultiDimensionalArray::rawIndexFromIndices] The array is empty.");
    assert(el.size() > 0 == dimensions().size() > 0 && "[matioCpp::MultiDimensionalArray::rawIndexFromIndices] The input vector el should have the same number of dimensions of the array.");
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
    if (!checkCompatibility(other.toMatio()))
    {
        return false;
    }

    return Variable::fromOther(other);
}

template<typename T>
bool matioCpp::MultiDimensionalArray<T>::fromOther(matioCpp::Variable &&other)
{
    if (!checkCompatibility(other.toMatio()))
    {
        return false;
    }

    return Variable::fromOther(other);
}

template<typename T>
bool matioCpp::MultiDimensionalArray<T>::fromMatio(const matvar_t *inputVar)
{
    if (!checkCompatibility(inputVar))
    {
        return false;
    }

    return Variable::fromMatio(inputVar);
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
