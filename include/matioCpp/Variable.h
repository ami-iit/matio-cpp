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
#include <matioCpp/Utilities.h>
#include <matioCpp/Span.h>

namespace matioCpp {
class Variable;
}

class matioCpp::Variable
{

    class Impl;

    std::unique_ptr<Impl> m_pimpl;

    bool createVar(const std::string &name, const VariableType &variableType, const ValueType &valueType, const std::vector<size_t> &dimensions, void *data);

    bool createComplexVar(const std::string& name, const VariableType& variableType, const ValueType& valueType, const std::vector<size_t>& dimensions, void *realData, void *imaginaryData);

protected:

    template<typename T>
    bool createVector(const std::string& name, const Span<T> inputVector)
    {
        return createVar(name, VariableType::Vector, get_type<T>::value_type, {inputVector.size(), 1}, (void*)inputVector.data());
    }

    template<typename T>
    bool createComplexVector(const std::string& name, const Span<T> realInputVector, const Span<T> imaginaryInputVector)
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
        return createComplexVar(name, VariableType::Vector, get_type<T>::value_type, {realInputVector.size(), 1}, (void*)realInputVector.data(), (void*)imaginaryInputVector.data());
    }

public:

    Variable();

    ~Variable();

    std::string name() const;

    matioCpp::VariableType variableType() const;

    matioCpp::ValueType valueType() const;

    bool isComplex() const;

};



#endif
