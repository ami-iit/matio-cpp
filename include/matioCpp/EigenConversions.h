/*
 * Copyright (C) 2022 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

#ifndef MATIOCPP_EIGENCONVERSIONS_H
#define MATIOCPP_EIGENCONVERSIONS_H

#include <matioCpp/Config.h>
#include <matioCpp/Vector.h>
#include <matioCpp/MultiDimensionalArray.h>

#include <type_traits>

#ifdef MATIOCPP_HAS_EIGEN

#include <Eigen/Core>

#endif

namespace matioCpp
{
/**
 * is_eigen_matrix is a template metafunction to check if T is an Eigen matrix.
 */
template <typename T, typename = void, typename = void>
struct is_eigen_matrix : std::false_type
{
};
}

#ifdef MATIOCPP_HAS_EIGEN

namespace matioCpp
{

/**
 * is_eigen_matrix is a template metafunction to check if T is an Eigen matrix. In this specialization, we first check if
 * the template parameter inherits from Eigen::MatrixBase<Derived> (Eigen exploits CRTP). If this is the case,
 * we check that neither the rows, nor the columns at compile time are identically equal to 1. If that is the case,
 * is_eigen_matrix<T>::value is true.
 */
template <typename Derived>
struct is_eigen_matrix<Derived,
                       typename std::enable_if_t<std::is_base_of<Eigen::MatrixBase<Derived>, Derived>::value>,
                       typename std::enable_if_t<Eigen::MatrixBase<Derived>::RowsAtCompileTime != 1 &&
                                                 Eigen::MatrixBase<Derived>::ColsAtCompileTime != 1>>: std::true_type
{
};

/**
 * @brief Conversion from a MultiDimensionalArray to an Eigen matrix
 * @param input The MultiDimensionalArray
 * @return A map from the internal data of the MultiDimensionalArray
 */
template <typename type>
Eigen::Map<Eigen::Matrix<type, Eigen::Dynamic, Eigen::Dynamic>> to_eigen(MultiDimensionalArray<type>& input);

/**
 * @brief Conversion from a const MultiDimensionalArray to an Eigen matrix
 * @param input The MultiDimensionalArray
 * @return A const map from the internal data of the MultiDimensionalArray
 */
template <typename type>
const Eigen::Map<Eigen::Matrix<type, Eigen::Dynamic, Eigen::Dynamic>> to_eigen(const MultiDimensionalArray<type>& input);

/**
 * @brief Conversion from a Vector to an Eigen vector
 * @param input The Vector
 * @return A map from the internal data of the Vector
 */
template <typename type>
Eigen::Map<Eigen::Matrix<type, Eigen::Dynamic, 1>> to_eigen(Vector<type>& input);

/**
 * @brief Conversion from a const Vector to an Eigen vector
 * @param input The Vector
 * @return A const map from the internal data of the Vector
 */
template <typename type>
const Eigen::Map<Eigen::Matrix<type, Eigen::Dynamic, 1>> to_eigen(const Vector<type>& input);

/**
 * @brief Conversion from an Eigen matrix to a MultiDimensionalArray
 * @param input The input matrix.
 * @param name The name of the resulting matioCpp variable.
 * @return A MultiDimensionalArray containing a copy of the input data
 */
template <typename EigenDerived, typename = std::enable_if_t<Eigen::MatrixBase<EigenDerived>::RowsAtCompileTime != 1 &&
                                                             Eigen::MatrixBase<EigenDerived>::ColsAtCompileTime != 1>>
MultiDimensionalArray<typename EigenDerived::Scalar> make_variable(const Eigen::MatrixBase<EigenDerived>& input, const std::string& name);

}

#include "impl/EigenConversions.tpp"

#endif


#endif // MATIOCPP_EIGENCONVERSIONS_H
