/*
 * Copyright (C) 2022 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

#ifndef EIGENCONVERSIONS_TPP
#define EIGENCONVERSIONS_TPP

#include <cassert>

template <typename type>
inline Eigen::Map<Eigen::Matrix<type, Eigen::Dynamic, Eigen::Dynamic>> matioCpp::to_eigen(matioCpp::MultiDimensionalArray<type>& input)
{
    assert(input.isValid());
    assert(input.dimensions().size() == 2);
    return Eigen::Map<Eigen::Matrix<type, Eigen::Dynamic, Eigen::Dynamic>>(input.data(), input.dimensions()(0), input.dimensions()(1));
}

template <typename type>
inline const Eigen::Map<Eigen::Matrix<type, Eigen::Dynamic, Eigen::Dynamic>> matioCpp::to_eigen(const matioCpp::MultiDimensionalArray<type>& input)
{
    assert(input.isValid());
    assert(input.dimensions().size() == 2);
    return Eigen::Map<const Eigen::Matrix<type, Eigen::Dynamic, Eigen::Dynamic>>(input.data(), input.dimensions()(0), input.dimensions()(1));
}

template <typename type>
inline Eigen::Map<Eigen::Matrix<type, Eigen::Dynamic, 1>> matioCpp::to_eigen(matioCpp::Vector<type>& input)
{
    assert(input.isValid());
    return Eigen::Map<Eigen::Matrix<type, Eigen::Dynamic, 1>>(input.data(), input.size());
}

template <typename type>
inline const Eigen::Map<Eigen::Matrix<type, Eigen::Dynamic, 1>> matioCpp::to_eigen(const matioCpp::Vector<type>& input)
{
    assert(input.isValid());
    return Eigen::Map<const Eigen::Matrix<type, Eigen::Dynamic, 1>>(input.data(), input.size());
}

template <typename EigenDerived, typename>
inline matioCpp::MultiDimensionalArray<typename EigenDerived::Scalar> matioCpp::make_variable(const std::string& name, const Eigen::MatrixBase<EigenDerived>& input)
{
    matioCpp::MultiDimensionalArray<typename EigenDerived::Scalar> matio(name, {static_cast<size_t>(input.rows()), static_cast<size_t>(input.cols())});
    matioCpp::to_eigen(matio) = input;
    return matio;
}

#endif // EIGENCONVERSIONS_TPP
