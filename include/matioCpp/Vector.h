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
#include <matioCpp/Utilities.h>
#include <matioCpp/Span.h>
#include <matioCpp/Variable.h>

template<typename T>
class matioCpp::Vector : public matioCpp::Variable
{

    bool createVector(const std::string& name, Span<T> inputVector);

public:

    //Typedefs to enable make_span.

    using value_type = T;

    using allocator_type = std::allocator<T>;

    using pointer = typename std::allocator_traits<std::allocator<T>>::pointer;

    using  const_pointer = typename std::allocator_traits<std::allocator<T>>::const_pointer;

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

    T * data();

    const T * data() const;

    T& operator()(size_t el);

    T operator()(size_t el) const;

    T& operator[](size_t el);

    T operator[](size_t el) const;

};

//#include "impl/Vector.tpp"
//---------------------------------implementation
template<typename T>
bool matioCpp::Vector<T>::createVector(const std::string& name, Span<T> inputVector)
{
    return createVar(name, VariableType::Vector, matioCpp::get_type<T>::valueType, {static_cast<size_t>(inputVector.size()), 1}, (void*)inputVector.data());
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
}

template<typename T>
matioCpp::Vector<T> &matioCpp::Vector<T>::operator=(matioCpp::Vector<T> &&other)
{
    fromOther(other);
}

template<typename T>
matioCpp::Vector<T> &matioCpp::Vector<T>::operator=(const Span<T> &other)
{
    if (size() == other.size())
    {
        memcpy(toMatio()->data, other.data(), size());
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
    //If there are at least two non-zero dimensions, take the biggest, otherwise the size is 0. Examples:
    // 0x0 -> 0
    //  0x1 -> 0
    // 1x1x0 -> 1
    // 1x0x0 -> 0
    // 1x1x0x0 -> 1

    size_t nonZeroDims = 0;
    for (size_t dim : this->dimensions())
    {
        nonZeroDims += dim > 0; //Count non-zero dimensions
    }

    return nonZeroDims > 1 ? *std::max_element(this->dimensions().begin(), this->dimensions().end()) : 0;
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



#endif // MATIOCPP_VECTOR_H
