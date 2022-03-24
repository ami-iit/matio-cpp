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
#include <matioCpp/Vector.h>
#include <matioCpp/MultiDimensionalArray.h>

#include "impl/ExogenousConversionHelpers.tpp"

namespace matioCpp
{

/**
 * @brief Conversion from a generic vector to a matioCpp::Vector
 * @param name The name of the resulting matioCpp variable.
 * @param input The input vector.
 * @return A matioCpp::Vector containing a copy of the input data
 */
template <class Vector, typename = typename std::enable_if_t<is_vector_compatible<Vector>::value &&
                                                             !std::is_same<Vector, std::string>::value>>
inline matioCpp::Vector<typename std::remove_cv_t<typename  matioCpp::SpanUtils::container_data<Vector>::type>> make_variable(const std::string& name, const Vector& input);

/**
 * @brief Conversion from a std::string to a matioCpp::String
 * @param name The name of the resulting matioCpp variable.
 * @param input The input string.
 * @return A matioCpp::String containing a copy of the input data
 */
inline matioCpp::String make_variable(const std::string& name, const std::string& input);

/**
 * @brief Conversion from a boolean vector to a matioCpp::Vector<matioCpp::Logical>
 * @param name The name of the resulting matioCpp variable.
 * @param input The input vector.
 * @return A matioCpp::Vector<matioCpp::Logical> containing a copy of the input data
 */
inline matioCpp::Vector<matioCpp::Logical> make_variable(const std::string& name, const std::vector<bool>& input);

/**
 * @brief Conversion from a fundamental type to the corresponding matioCpp::Element
 * @param name The name of the resulting matioCpp variable.
 * @param input The input element.
 * @return A matioCpp::Element containing a copy of the input data
 */
template<typename type, typename = typename std::enable_if<std::is_fundamental<type>::value && !std::is_same<type, bool>::value>::type>
inline matioCpp::Element<type> make_variable(const std::string& name, const type& input);

/**
 * @brief Conversion from a boolean to a matioCpp::Element<matioCpp::Logical>
 * @param name The name of the resulting matioCpp variable.
 * @param input The input element.
 * @return A matioCpp::Element<matioCpp::Logical> whose value is equal to the input.
 */
inline matioCpp::Element<matioCpp::Logical> make_variable(const std::string& name, bool input);

/**
 * @brief Conversion from a vector of strings to a matioCpp::CellArray containing the input strings
 * @param name The name of the resulting matioCpp variable.
 * @param input The input vector of strings.
 * @return A matioCpp::CellArray of dimensions nx1 (with n the number of strings)
 */
inline matioCpp::CellArray make_variable(const std::string& name, const std::vector<std::string>& input);

/**
 * @brief Create a matioCpp::Struct starting from the begin and end iterators of a map-like container
 * The dereferenced value of the iterator has to be a pair (like with std::maps and std::unordered_map)
 * with the key being a string. For each key, there is the corresponding field in the Struct.
 * @param name The name of the struct.
 * @param begin The iterator to the first element
 * @param end The iterator to the element after the last.
 * @return The corresponding matioCpp::Struct
 */
template<class iterator,
          typename = typename std::enable_if_t<is_pair_iterator_string<iterator>::value>>
inline matioCpp::Struct make_struct(const std::string& name, iterator begin, iterator end);

/**
 * @brief Create a matioCpp::CellArray starting from the begin and end iterators of a container.
 * It need the dereferenced value of the iterator to be a pair (like with std::maps and std::unordered_map).
 * The first element is used as name, while the second is the value.
 * @param name The name of the CellArray.
 * @param begin The iterator to the first element
 * @param end The iterator to the element after the last.
 * @return The corresponding matioCpp::CellArray.
 */
template<class iterator,
         typename std::enable_if_t<is_pair_iterator_string<iterator>::value>* = nullptr>
inline matioCpp::CellArray make_cell_array(const std::string& name, const iterator& begin, const iterator& end);

/**
 * @brief Create a matioCpp::CellArray starting from the begin and end iterators of a container.
 * The name of the imported variable in the CellArray
 * is "imported_element_x", where "x" is the corresponding raw index.
 * @param name The name of the CellArray.
 * @param begin The iterator to the first element
 * @param end The iterator to the element after the last.
 * @return The corresponding matioCpp::CellArray.
 */
template<class iterator,
         typename std::enable_if_t<!is_pair<decltype(*std::declval<iterator>())>::value>* = nullptr>
inline matioCpp::CellArray make_cell_array(const std::string& name, const iterator& begin, const iterator& end);

}

#include "impl/ExogenousConversions.tpp"

#endif // MATIOCPP_EXOGENOUSCONVERSION_H
