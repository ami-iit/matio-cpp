/*
 * Copyright (C) 2022 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */
#ifndef MATIOCPP_EXOGENOUSCONVERSIONHELPERS_TPP
#define MATIOCPP_EXOGENOUSCONVERSIONHELPERS_TPP

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
 * Template metafunction to check if the input type is a pair
 */
template <typename, typename = void, typename = void>
struct is_pair : std::false_type
{ };

/**
 * Template metafunction to check if the input type is a pair
 */
template <typename T>
struct is_pair<T, matioCpp::SpanUtils::void_t<decltype(std::declval<T>().first)>, matioCpp::SpanUtils::void_t<decltype(std::declval<T>().second)>> : std::true_type
{ };

/**
 * Template metafunction to check if the input type is a pair iterator with a string as first element.
 */
template <typename, typename = void, typename = void>
struct is_pair_iterator_string : std::false_type
{ };

/**
 * Template metafunction to check if the input type is a pair iterator with a string as first element.
 */
template <typename T>
struct is_pair_iterator_string<T,
                               typename std::enable_if_t<is_pair<decltype(*std::declval<T>())>::value>,
                               typename std::enable_if_t<std::is_convertible<decltype(std::declval<T>()->first), std::string>::value>> : std::true_type
{ };

}

#endif // MATIOCPP_EXOGENOUSCONVERSIONHELPERS_TPP
