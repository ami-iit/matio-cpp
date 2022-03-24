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

#include "impl/ExogenousConversionHelpers.tpp"

namespace matioCpp
{

/**
 * @brief Conversion from a generic vector to a matioCpp::Vector
 * @param input The input vector.
 * @param name The name of the resulting matioCpp variable.
 * @return A matioCpp::Vector containing a copy of the input data
 */
template <class Vector, typename = typename std::enable_if_t<is_vector_compatible<Vector>::value &&
                                                             !std::is_same<Vector, std::string>::value>>
matioCpp::Vector<typename std::remove_cv_t<typename  matioCpp::SpanUtils::container_data<Vector>::type>> make_variable(const Vector& input, const std::string& name);

/**
 * @brief Conversion from a std::string to a matioCpp::String
 * @param input The input string.
 * @param name The name of the resulting matioCpp variable.
 * @return A matioCpp::String containing a copy of the input data
 */
inline matioCpp::String make_variable(const std::string& input, const std::string& name);

/**
 * @brief Conversion from a boolean vector to a matioCpp::Vector<matioCpp::Logical>
 * @param input The input vector.
 * @param name The name of the resulting matioCpp variable.
 * @return A matioCpp::Vector<matioCpp::Logical> containing a copy of the input data
 */
inline matioCpp::Vector<matioCpp::Logical> make_variable(const std::vector<bool>& input, const std::string& name);

/**
 * @brief Conversion from a fundamental type to the corresponding matioCpp::Element
 * @param input The input element.
 * @param name The name of the resulting matioCpp variable.
 * @return A matioCpp::Element containing a copy of the input data
 */
template<typename type, typename = typename std::enable_if<std::is_fundamental<type>::value && !std::is_same<type, bool>::value>::type>
matioCpp::Element<type> make_variable(const type& input, const std::string& name);

/**
 * @brief Conversion from a boolean to a matioCpp::Element<matioCpp::Logical>
 * @param input The input element.
 * @param name The name of the resulting matioCpp variable.
 * @return A matioCpp::Element<matioCpp::Logical> whose value is equal to the input.
 */
inline matioCpp::Element<matioCpp::Logical> make_variable(bool input, const std::string& name);

/**
 * @brief Conversion from a vector of strings to a matioCpp::CellArray containing the input strings
 * @param input The input vector of strings.
 * @param name The name of the resulting matioCpp variable.
 * @return A matioCpp::CellArray of dimensions nx1 (with n the number of strings)
 */
inline matioCpp::CellArray make_variable(const std::vector<std::string>& input, const std::string& name);

/**
 * @brief Create a matioCpp::Struct starting from the begin and end iterators of a map-like container
 * The dereferenced value of the iterator has to be a pair (like with std::maps and std::unordered_map)
 * with the key being a string. For each key, there is the corresponding field in the Struct.
 * @param begin The iterator to the first element
 * @param end The iterator to the element after the last.
 * @param name The name of the struct.
 * @return The corresponding matioCpp::Struct
 */
template<class iterator,
          typename = typename std::enable_if_t<is_pair_iterator_string<iterator>::value>>
matioCpp::Struct make_struct(iterator begin, iterator end, const std::string& name);

/**
 * @brief Create a matioCpp::CellArray starting from the begin and end iterators of a container.
 * It need the dereferenced value of the iterator to be a pair (like with std::maps and std::unordered_map).
 * The first element is used as name, while the second is the value.
 * @param begin The iterator to the first element
 * @param end The iterator to the element after the last.
 * @param name The name of the CellArray.
 * @return The corresponding matioCpp::CellArray.
 */
template<class iterator,
         typename std::enable_if_t<is_pair_iterator_string<iterator>::value>* = nullptr>
matioCpp::CellArray make_cell_array(const iterator& begin, const iterator& end, const std::string& name);

/**
 * @brief Create a matioCpp::CellArray starting from the begin and end iterators of a container.
 * The name of the imported variable in the CellArray
 * is "imported_element_x", where "x" is the corresponding raw index.
 * @param begin The iterator to the first element
 * @param end The iterator to the element after the last.
 * @param name The name of the CellArray.
 * @return The corresponding matioCpp::CellArray.
 */
template<class iterator,
         typename std::enable_if_t<!is_pair<decltype(*std::declval<iterator>())>::value>* = nullptr>
matioCpp::CellArray make_cell_array(const iterator& begin, const iterator& end, const std::string& name);

}

#include "impl/ExogenousConversions.tpp"

#endif // MATIOCPP_EXOGENOUSCONVERSION_H
