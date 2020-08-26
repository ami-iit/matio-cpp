#ifndef MATIOCPP_VECTOR_H
#define MATIOCPP_VECTOR_H

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
#include <matioCpp/VectorIterator.h>

template<typename T>
class matioCpp::Vector : public matioCpp::Variable
{

    bool createVector(const std::string& name, Span<T> inputVector);

public:

    using element_type = T;

    using value_type = std::remove_cv_t<T>;

    using allocator_type = std::allocator<T>;

    using index_type = size_t;

    using pointer = typename std::allocator_traits<std::allocator<T>>::pointer;

    using const_pointer = typename std::allocator_traits<std::allocator<T>>::const_pointer;

    using iterator = vector_iterator<Vector<T>, false>;

    using const_iterator = vector_iterator<Vector<T>, true>;

    using reverse_iterator = std::reverse_iterator<iterator>;

    using const_reverse_iterator =std::reverse_iterator<const_iterator>;

    Vector();

    Vector(const std::string& name);

    Vector(const std::string& name, Span<T> inputVector);

    Vector(const Vector<T>& other);

    Vector(Vector<T>&& other);

    ~Vector();

    Vector<T>& operator=(const Vector<T>& other);

    Vector<T>& operator=(Vector<T>&& other);

    Vector<T>& operator=(const Span<T>& other);

    virtual bool fromOther(const Variable& other) final;

    virtual bool fromOther(Variable&& other) final;

    virtual bool fromMatio(const matvar_t * inputVar) final;

    matioCpp::Span<T> toSpan();

    const matioCpp::Span<const T> toSpan() const;

    bool setName(const std::string& newName);

    size_t size() const;

    void resize(size_t newSize);

    T * data();

    const T * data() const;

    T& operator()(size_t el);

    T operator()(size_t el) const;

    T& operator[](size_t el);

    T operator[](size_t el) const;

    iterator begin();

    iterator end();

    const_iterator begin() const;

    const_iterator end() const;

    const_iterator cbegin() const;

    const_iterator cend() const;

    reverse_iterator rbegin();

    reverse_iterator rend();

    const_reverse_iterator rbegin() const;

    const_reverse_iterator rend() const;

    const_reverse_iterator crbegin() const;

    const_reverse_iterator crend() const;
};

//#include "impl/Vector.tpp"
//---------------------------------implementation
template<typename T>
bool matioCpp::Vector<T>::createVector(const std::string& name, Span<T> inputVector)
{
    return createVar(name, VariableType::Vector, matioCpp::get_type<T>::valueType, {static_cast<size_t>(inputVector.size()), 1}, (void*)inputVector.data());
}

template<typename T>
matioCpp::Vector<T>::Vector()
{
    static_assert (!std::is_same<T, bool>::value, "Vector<bool> is not supported." );
    std::vector<T> empty;
    createVector("unnamed_vector", matioCpp::make_span(empty));
}

template<typename T>
matioCpp::Vector<T>::Vector(const std::string& name)
{
    static_assert (!std::is_same<T, bool>::value, "Vector<bool> is not supported." );
    std::vector<T> empty;
    createVector(name, matioCpp::make_span(empty));
}

template<typename T>
matioCpp::Vector<T>::Vector(const std::string& name, Span<T> inputVector)
{
    static_assert (!std::is_same<T, bool>::value, "Vector<bool> is not supported." );
    createVector(name, inputVector);
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
        createVector(name(), other);
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
    return createVector(newName, toSpan());
}

template<typename T>
size_t matioCpp::Vector<T>::size() const
{
    //A vector should have the size of dimensions equal to 2
    assert(this->dimensions().size() == 2);

    return std::min(this->dimensions()[0], this->dimensions()[1]) > 0 ? std::max(this->dimensions()[0], this->dimensions()[1]) : 0;
}

template<typename T>
void matioCpp::Vector<T>::resize(size_t newSize)
{
    if (newSize != size())
    {
        std::vector<T> newVector(newSize);
        memcpy(newVector.data(), data(), std::min(newSize, size()) * sizeof(T));
        this->operator=(newVector);
    }
}

template<typename T>
T *matioCpp::Vector<T>::data()
{
    return static_cast<T*>(toMatio()->data);
}

template<typename T>
const T *matioCpp::Vector<T>::data() const
{
    return static_cast<const T*>(toMatio()->data);
}

template<typename T>
T &matioCpp::Vector<T>::operator()(size_t el)
{
    return toSpan()(el);
}

template<typename T>
T matioCpp::Vector<T>::operator()(size_t el) const
{
    return toSpan()(el);
}

template<typename T>
T &matioCpp::Vector<T>::operator[](size_t el)
{
    return this->operator()(el);
}

template<typename T>
T matioCpp::Vector<T>::operator[](size_t el) const
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


#endif // MATIOCPP_VECTOR_H
