#ifndef MATIOCPP_VECTOR_TPP
#define MATIOCPP_VECTOR_TPP

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

template<typename T>
bool matioCpp::Vector<T>::initializeVector(const std::string& name, Span<T> inputVector)
{
    size_t dimensions[] = {1, static_cast<size_t>(inputVector.size())};
    return initializeVariable(name, VariableType::Vector, matioCpp::get_type<T>::valueType(), dimensions, (void*)inputVector.data());
}

template<typename T>
bool matioCpp::Vector<T>::checkCompatibility(const matvar_t* inputPtr, matioCpp::VariableType variableType, matioCpp::ValueType valueType) const
{

    if ((variableType != matioCpp::VariableType::Vector) &&
        (variableType != matioCpp::VariableType::Element))
    {
        std::cerr << "[matioCpp::Vector::checkCompatibility] The variable type is not compatible with a vector." << std::endl;
        return false;
    }

    if (inputPtr->isComplex)
    {
        std::cerr << "[matioCpp::Vector::checkCompatibility] Cannot use a complex variable into a non-complex one." << std::endl;
        return false;
    }

    if (!matioCpp::is_convertible_to_primitive_type<T>(valueType))
    {
        std::string dataType = "";
        std::string classType = "";

        get_types_names_from_matvart(inputPtr, classType, dataType);

        std::cerr << "[matioCpp::Vector::checkCompatibility] The value type is not convertible to " <<
            get_type<T>::toString() <<"." << std::endl <<
            "                                       Input class type: " << classType << std::endl <<
            "                                       Input data type: " << dataType << std::endl;
        return false;
    }
    return true;
}

template<typename T>
matioCpp::Vector<T>::Vector()
{
    static_assert (!std::is_same<T, bool>::value, "Vector<bool> is not supported." );
    std::vector<T> empty;
    initializeVector("unnamed_vector", matioCpp::make_span(empty));
}

template<typename T>
matioCpp::Vector<T>::Vector(const std::string& name)
{
    static_assert (!std::is_same<T, bool>::value, "Vector<bool> is not supported." );
    std::vector<T> empty;

    if (std::is_same<T, char>::value) //If the type is char, the name corresponds to the content
    {
        empty.resize(name.size());
        for (size_t i = 0; i < name.size(); ++i)
        {
            empty[i] = name[i];
        }
    }

    initializeVector(name, matioCpp::make_span(empty));
}

template<typename T>
matioCpp::Vector<T>::Vector(const std::string &name, matioCpp::Vector<T>::index_type dimensions)
{
    static_assert (!std::is_same<T, bool>::value, "Vector<bool> is not supported." );
    std::vector<T> empty(dimensions);
    initializeVector(name, matioCpp::make_span(empty));
}

template<typename T>
matioCpp::Vector<T>::Vector(const std::string& name, Span<T> inputVector)
{
    static_assert (!std::is_same<T, bool>::value, "Vector<bool> is not supported." );
    initializeVector(name, inputVector);
}

template <typename T>
matioCpp::Vector<T>::Vector(const std::string &name, const std::string &inputString)
{
    static_assert (std::is_same<T, char>::value,"The assignement operator from a string is available only if the type of the vector is char");
    size_t dimensions[] = {1, static_cast<size_t>(inputString.size())};
    initializeVariable(name, VariableType::Vector, matioCpp::get_type<T>::valueType(), dimensions, (void*)inputString.c_str());
}

template<typename T>
matioCpp::Vector<T>::Vector(const Vector<T> &other)
{
    fromOther(other);
}

template<typename T>
matioCpp::Vector<T>::Vector(Vector<T> &&other)
{
    fromOther(std::forward<matioCpp::Vector<T>>(other));
}

template<typename T>
matioCpp::Vector<T>::Vector(const MatvarHandler &handler)
    : matioCpp::Variable(handler)
{
    static_assert (!std::is_same<T, bool>::value, "Vector<bool> is not supported." );

    if (!handler.get() || !checkCompatibility(handler.get(), handler.variableType(), handler.valueType()))
    {
        assert(false);
        std::vector<T> empty;
        initializeVector("unnamed_vector", matioCpp::make_span(empty));
    }
}

template<typename T>
matioCpp::Vector<T>::~Vector()
{

}

template<typename T>
matioCpp::Vector<T> &matioCpp::Vector<T>::operator=(const matioCpp::Vector<T> &other)
{
    fromOther(other);
    return *this;
}

template<typename T>
matioCpp::Vector<T> &matioCpp::Vector<T>::operator=(matioCpp::Vector<T> &&other)
{
    fromOther(std::forward<matioCpp::Vector<T>>(other));
    return *this;
}

template<typename T>
matioCpp::Vector<T> &matioCpp::Vector<T>::operator=(const Span<T> &other)
{
    if (size() == other.size())
    {
        memcpy(toMatio()->data, other.data(), size() * sizeof(T));
    }
    else
    {
        bool ok = initializeVector(name(), other);
        assert(ok && "Failed to resize.");
    }

    return *this;
}

template<typename T>
matioCpp::Vector<T> &matioCpp::Vector<T>::operator=(const std::string &other)
{
    static_assert (std::is_same<T, char>::value,"The assignement operator from a string is available only if the type of the vector is char");
    if (size() == other.size())
    {
        memcpy(toMatio()->data, other.data(), size() * sizeof(T));
    }
    else
    {
        size_t dimensions[] = {1, static_cast<size_t>(other.size())};
        initializeVariable(name(), VariableType::Vector, matioCpp::get_type<T>::valueType(), dimensions, (void*)other.c_str());
    }

    return *this;

}

template<typename T>
matioCpp::Span<T> matioCpp::Vector<T>::toSpan()
{
    return matioCpp::make_span(*this);
}

template<typename T>
const matioCpp::Span<const T> matioCpp::Vector<T>::toSpan() const
{
    return matioCpp::make_span(*this);
}

template<typename T>
bool matioCpp::Vector<T>::setName(const std::string &newName)
{
    return changeName(newName);
}

template<typename T>
typename matioCpp::Vector<T>::index_type matioCpp::Vector<T>::size() const
{
    //A vector should have the size of dimensions equal to 2
    assert(this->dimensions().size() == 2);

    return std::min(this->dimensions()[0], this->dimensions()[1]) > 0 ? std::max(this->dimensions()[0], this->dimensions()[1]) : 0;
}

template<typename T>
void matioCpp::Vector<T>::resize(typename matioCpp::Vector<T>::index_type newSize)
{
    if (newSize != size())
    {
        std::vector<T> newVector(newSize);
        memcpy(newVector.data(), data(), std::min(newSize, size()) * sizeof(T));
        this->operator=(newVector);
    }
}

template<typename T>
void matioCpp::Vector<T>::clear()
{
    fromOther(std::move(Vector<T>(name())));
}

template<typename T>
typename matioCpp::Vector<T>::pointer matioCpp::Vector<T>::data()
{
    return static_cast<T*>(toMatio()->data);
}

template<typename T>
typename matioCpp::Vector<T>::const_pointer matioCpp::Vector<T>::data() const
{
    return static_cast<const T*>(toMatio()->data);
}

template<typename T>
typename matioCpp::Vector<T>::reference matioCpp::Vector<T>::operator()(typename matioCpp::Vector<T>::index_type el)
{
    return toSpan()(el);
}

template<typename T>
typename matioCpp::Vector<T>::value_type matioCpp::Vector<T>::operator()(typename matioCpp::Vector<T>::index_type el) const
{
    return toSpan()(el);
}

template<typename T>
std::string matioCpp::Vector<T>::operator()() const
{
    static_assert (std::is_same<T, char>::value,"The operator () to convert to a string is available only if the type of the vector is char");
    return std::string(data(), size());
}

template<typename T>
typename matioCpp::Vector<T>::reference matioCpp::Vector<T>::operator[](typename matioCpp::Vector<T>::index_type el)
{
    return this->operator()(el);
}

template<typename T>
typename matioCpp::Vector<T>::value_type matioCpp::Vector<T>::operator[](typename matioCpp::Vector<T>::index_type el) const
{
    return this->operator()(el);
}

template<typename T>
typename matioCpp::Vector<T>::iterator matioCpp::Vector<T>::begin()
{
    return iterator(this, 0);
}

template<typename T>
typename matioCpp::Vector<T>::iterator matioCpp::Vector<T>::end()
{
    return iterator(this, size());
}

template<typename T>
typename matioCpp::Vector<T>::const_iterator matioCpp::Vector<T>::begin() const
{
    return const_iterator(this, 0);
}

template<typename T>
typename matioCpp::Vector<T>::const_iterator matioCpp::Vector<T>::end() const
{
    return const_iterator(this, size());
}

template<typename T>
typename matioCpp::Vector<T>::const_iterator matioCpp::Vector<T>::cbegin() const
{
    return const_iterator(this, 0);
}

template<typename T>
typename matioCpp::Vector<T>::const_iterator matioCpp::Vector<T>::cend() const
{
    return const_iterator(this, size());
}

template<typename T>
typename matioCpp::Vector<T>::reverse_iterator matioCpp::Vector<T>::rbegin()
{
    return reverse_iterator{end()};
}

template<typename T>
typename matioCpp::Vector<T>::reverse_iterator matioCpp::Vector<T>::rend()
{
    return reverse_iterator{begin()};
}

template<typename T>
typename matioCpp::Vector<T>::const_reverse_iterator matioCpp::Vector<T>::rbegin() const
{
    return const_reverse_iterator{cend()};
}

template<typename T>
typename matioCpp::Vector<T>::const_reverse_iterator matioCpp::Vector<T>::rend() const
{
    return const_reverse_iterator{cbegin()};
}

template<typename T>
typename matioCpp::Vector<T>::const_reverse_iterator matioCpp::Vector<T>::crbegin() const
{
    return const_reverse_iterator{cend()};
}

template<typename T>
typename matioCpp::Vector<T>::const_reverse_iterator matioCpp::Vector<T>::crend() const
{
    return const_reverse_iterator{cbegin()};
}

template<typename T>
matioCpp::Vector<T> matioCpp::Variable::asVector()
{
    return matioCpp::Vector<T>(*m_handler);
}

template<typename T>
const matioCpp::Vector<T> matioCpp::Variable::asVector() const
{
    return matioCpp::Vector<T>(*m_handler);
}

matioCpp::String matioCpp::Variable::asString()
{
    return matioCpp::Vector<char>(*m_handler);
}

const matioCpp::String matioCpp::Variable::asString() const
{
    return matioCpp::Vector<char>(*m_handler);
}

#endif // MATIOCPP_VECTOR_TPP
