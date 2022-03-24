/*
 * Copyright (C) 2022 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */
#ifndef MATIOCPP_EXOGENOUSCONVERSION_H
#define MATIOCPP_EXOGENOUSCONVERSION_H

#include <matioCpp/ForwardDeclarations.h>
#include <matioCpp/EigenConversions.h>
#include <matioCpp/Element.h>
#include <matioCpp/Struct.h>
#include <matioCpp/CellArray.h>
#include <matioCpp/Span.h>

namespace matioCpp
{

/**
 * is_vector_compatible is a utility metafunction to check if the input vector T is compatible with matioCpp
 */
template <typename T, typename = void, typename = void>
struct is_vector_compatible : std::false_type
{
};

/**
 * is_vector_compatible is a utility metafunction to check if the input vector T is compatible with matioCpp
 * This specialization first checks if T is an array, or if it is possible to deduce the type of vector,
 * and that it is not an Eigen matrix (Eigen defines the size() method also for matrices).
 * If not, this specialization is not used (SFINAE). Otherwise, given the vector type, it checks if a matioCpp::Span is construbile.
 * In addition, the type has not to be <code>bool<\code>.
 * If all the above checks are true, <code>is_vector_compatible<T>::value = true<\code>.
 */
template <typename T>
    struct is_vector_compatible<T,
                               typename std::enable_if<(std::is_array<T>::value || matioCpp::SpanUtils::has_type_member<T>::value || matioCpp::SpanUtils::has_data_method<T>::value) && !is_eigen_matrix<T>::value>::type,
    typename std::enable_if<matioCpp::SpanUtils::is_make_span_callable<T>::value &&
                            !std::is_same<typename matioCpp::SpanUtils::container_data<T>::type, bool>::value>::type> : std::true_type
{
};

/**
 * @brief Conversion from a generic vector to a matioCpp::Vector
 * @param input The input vector.
 * @param name The name of the resulting matioCpp variable.
 * @return A matioCpp::Vector containing a copy of the input data
 */
template <class Vector, typename = typename std::enable_if_t<is_vector_compatible<Vector>::value &&
                                                             !std::is_same<Vector, std::string>::value>>
matioCpp::Vector<typename std::remove_cv_t<typename  matioCpp::SpanUtils::container_data<Vector>::type>> make_variable(const Vector& input, const std::string& name)
{
    using type = typename  matioCpp::SpanUtils::container_data<Vector>::type;
    return matioCpp::Vector<typename std::remove_cv_t<type>>(name, matioCpp::make_span(input)); //data is copied
}

/**
 * @brief Conversion from a std::string to a matioCpp::String
 * @param input The input string.
 * @param name The name of the resulting matioCpp variable.
 * @return A matioCpp::String containing a copy of the input data
 */
inline matioCpp::String make_variable(const std::string& input, const std::string& name)
{
    return matioCpp::String(name, input);
}

/**
 * @brief Conversion from a boolean vector to a matioCpp::Vector<matioCpp::Logical>
 * @param input The input vector.
 * @param name The name of the resulting matioCpp variable.
 * @return A matioCpp::Vector<matioCpp::Logical> containing a copy of the input data
 */
inline matioCpp::Vector<matioCpp::Logical> make_variable(const std::vector<bool>& input, const std::string& name)
{
    return matioCpp::Vector<matioCpp::Logical>(name, input);
}

/**
 * @brief Conversion from a fundamental type to the corresponding matioCpp::Element
 * @param input The input element.
 * @param name The name of the resulting matioCpp variable.
 * @return A matioCpp::Element containing a copy of the input data
 */
template<typename type, typename = typename std::enable_if<std::is_fundamental<type>::value && !std::is_same<type, bool>::value>::type>
matioCpp::Element<type> make_variable(const type& input, const std::string& name)
{
    return matioCpp::Element<type>(name, input);
}

/**
 * @brief Conversion from a boolean to a matioCpp::Element<matioCpp::Logical>
 * @param input The input element.
 * @param name The name of the resulting matioCpp variable.
 * @return A matioCpp::Element<matioCpp::Logical> whose value is equal to the input.
 */
inline matioCpp::Element<matioCpp::Logical> make_variable(bool input, const std::string& name)
{
    return matioCpp::Element<matioCpp::Logical>(name, input);
}

/**
 * @brief Conversion from a vector of strings to a matioCpp::CellArray containing the input strings
 * @param input The input vector of strings.
 * @param name The name of the resulting matioCpp variable.
 * @return A matioCpp::CellArray of dimensions nx1 (with n the number of strings)
 */
inline matioCpp::CellArray make_variable(const std::vector<std::string>& input, const std::string& name)
{
    matioCpp::CellArray stringsArray(name, {input.size(), 1});
    for (size_t i = 0; i < input.size(); ++i)
    {
        stringsArray.setElement(i, make_variable(input[i], input[i]));
    }

    return stringsArray;
}

}


#endif // MATIOCPP_EXOGENOUSCONVERSION_H
