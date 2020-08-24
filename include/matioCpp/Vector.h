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

    Vector(const std::string& name);

    Vector(const std::string& name, Span<T> inputVector);

    ~Vector();

    size_t size() const;

};

//#include "impl/Vector.tpp"
//---------------------------------implementation
template<typename T>
bool matioCpp::Vector<T>::createVector(const std::string& name, Span<T> inputVector)
{
    return createVar(name, VariableType::Vector, get_type<T>::value_type, {inputVector.size(), 1}, (void*)inputVector.data());
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
size_t matioCpp::Vector<T>::size() const
{
    return *std::max_element(this->dimensions().begin(), this->dimensions().end());
}



#endif // MATIOCPP_VECTOR_H
