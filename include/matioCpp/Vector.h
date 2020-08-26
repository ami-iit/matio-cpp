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

#include "impl/Vector.tpp"


#endif // MATIOCPP_VECTOR_H
