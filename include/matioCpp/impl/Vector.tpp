#ifndef MATIOCPP_VECTOR_TPP
#define MATIOCPP_VECTOR_TPP

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
bool matioCpp::Vector<T>::initializeVector(const std::string& name, Span<T> inputVector)
{
    return initializeVariable(name, VariableType::Vector, matioCpp::get_type<T>::valueType, {static_cast<size_t>(inputVector.size()), 1}, (void*)inputVector.data());
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
    initializeVector(name, matioCpp::make_span(empty));
}

template<typename T>
matioCpp::Vector<T>::Vector(const std::string& name, Span<T> inputVector)
{
    static_assert (!std::is_same<T, bool>::value, "Vector<bool> is not supported." );
    initializeVector(name, inputVector);
}

template<typename T>
matioCpp::Vector<T>::Vector(const Vector<T> &other)
{
    fromOther(other);
}

template<typename T>
matioCpp::Vector<T>::Vector(Vector<T> &&other)
{
    fromOther(other);
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
    fromOther(other);
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
        initializeVector(name(), other);
    }

    return *this;
}

template<typename T>
bool matioCpp::Vector<T>::fromOther(const matioCpp::Variable &other)
{
    if (other.variableType() != matioCpp::VariableType::Vector)
    {
        std::cerr << "[matioCpp::Vector::fromOther] The input variable is not a vector." << std::endl;
        return false;
    }

    if (!matioCpp::is_convertible_to_primitive_type<T>(other.valueType()))
    {
        std::cerr << "[matioCpp::Vector::fromOther] The input type is not convertible to " <<
            typeid(T).name() <<"." << std::endl;
        return false;
    }
    return Variable::fromOther(other);
}

template<typename T>
bool matioCpp::Vector<T>::fromOther(matioCpp::Variable &&other)
{
    if (other.variableType() != matioCpp::VariableType::Vector)
    {
        std::cerr << "[matioCpp::Vector::fromOther] The input variable is not a vector." << std::endl;
        return false;
    }

    if (!matioCpp::is_convertible_to_primitive_type<T>(other.valueType()))
    {
        std::cerr << "[matioCpp::Vector::fromOther] The input type is not convertible to " <<
            typeid(T).name() <<"." << std::endl;
        return false;
    }
    return Variable::fromOther(other);
}

template<typename T>
bool matioCpp::Vector<T>::fromMatio(const matvar_t *inputVar)
{
    Variable dummy;
    if (!dummy.fromMatio(inputVar))
    {
        return false;
    }

    return fromOther(dummy);
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
    return initializeVector(newName, toSpan());
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




#endif // MATIOCPP_VECTOR_TPP
