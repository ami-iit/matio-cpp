/*
 * Copyright (C) 2022 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

#ifndef EIGENCONVERSIONS_TPP
#define EIGENCONVERSIONS_TPP

#include <cassert>

namespace matioCpp
{
    struct SlicingInfo
    {
        std::pair<size_t, size_t> dimensions{0,0};
        size_t offset{ 0 };
        size_t innerStride{ 0 };
        size_t outerStride{ 0 };
        bool valid{ false };
    };

    template <typename type>
    SlicingInfo computeSlicingInfo(const matioCpp::MultiDimensionalArray<type>& input, const std::vector<int>& slice)
    {
        std::string errorPrefix = "[ERROR][matioCpp::to_eigen] ";

        using index_type = typename matioCpp::MultiDimensionalArray<type>::index_type;
        SlicingInfo info;

        const auto& dimensions = input.dimensions();

        if (slice.size() != dimensions.size())
        {
            std::cerr << errorPrefix << "The number of slices must be equal to the number of dimensions of the input MultiDimensionalArray" << std::endl;
            assert(false);
            return info;
        }

        std::vector<index_type> startingIndex(slice.size(), 0);
        index_type previousDimensionsFactorial = 1;

        for (size_t i = 0; i < slice.size(); ++i)
        {
            if (slice[i] >= 0)
            {
                if (slice[i] >= dimensions(i))
                {
                    std::cerr << errorPrefix << "The slice is larger than the dimension of the input MultiDimensionalArray" << std::endl;
                    assert(false);
                    return SlicingInfo();
                }
                startingIndex[i] = static_cast<size_t>(slice[i]);
            }
            else
            {
                if (info.dimensions.first == 0)
                {
                    info.dimensions.first = dimensions(i);
                    info.innerStride = previousDimensionsFactorial;
                }
                else if (info.dimensions.second == 0)
                {
                    info.dimensions.second = dimensions(i);
                    info.outerStride = previousDimensionsFactorial;
                }
                else
                {
                    std::cerr << errorPrefix << "Only at most two free dimensions are allowed" << std::endl;
                    assert(false);
                    return SlicingInfo();
                }
            }
            previousDimensionsFactorial *= dimensions(i);
        }

        if (info.dimensions.first == 0) //Single element
        {
            info.dimensions.first = 1;
            info.innerStride = 1;
        }

        if (info.dimensions.second == 0) //Vector case
        {
            info.dimensions.second = 1;
            info.outerStride = info.dimensions.first * info.innerStride;
        }

        info.offset = input.rawIndexFromIndices(startingIndex);

        info.valid = true;
        return info;

    }
}

template <typename type>
inline Eigen::Map<Eigen::Matrix<type, Eigen::Dynamic, Eigen::Dynamic>> matioCpp::to_eigen(matioCpp::MultiDimensionalArray<type>& input)
{
    assert(input.isValid());
    assert(input.dimensions().size() == 2);
    return Eigen::Map<Eigen::Matrix<type, Eigen::Dynamic, Eigen::Dynamic>>(input.data(), input.dimensions()(0), input.dimensions()(1));
}

template <typename type>
inline Eigen::Map<const Eigen::Matrix<type, Eigen::Dynamic, Eigen::Dynamic>> matioCpp::to_eigen(const matioCpp::MultiDimensionalArray<type>& input)
{
    assert(input.isValid());
    assert(input.dimensions().size() == 2);
    return Eigen::Map<const Eigen::Matrix<type, Eigen::Dynamic, Eigen::Dynamic>>(input.data(), input.dimensions()(0), input.dimensions()(1));
}

template <typename type>
inline matioCpp::EigenMapWithStride<type> matioCpp::to_eigen(matioCpp::MultiDimensionalArray<type>& input, const std::vector<int>& slice)
{
    assert(input.isValid());

    auto slicingInfo = computeSlicingInfo(input, slice);
    Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic> stride(slicingInfo.outerStride, slicingInfo.innerStride);
    if (!slicingInfo.valid)
    {
        return matioCpp::EigenMapWithStride<type>(nullptr, 0, 0, stride);
    }
    return matioCpp::EigenMapWithStride<type>(input.data() + slicingInfo.offset, slicingInfo.dimensions.first, slicingInfo.dimensions.second, stride);
}

template <typename type>
inline matioCpp::ConstEigenMapWithStride<type> matioCpp::to_eigen(const matioCpp::MultiDimensionalArray<type>& input, const std::vector<int>& slice)
{
    assert(input.isValid());

    auto slicingInfo = computeSlicingInfo(input, slice);
    Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic> stride(slicingInfo.outerStride, slicingInfo.innerStride);


    if (!slicingInfo.valid)
    {
        return ConstEigenMapWithStride<type>(nullptr, 0, 0, stride);
    }

    return ConstEigenMapWithStride<type>(input.data() + slicingInfo.offset, slicingInfo.dimensions.first, slicingInfo.dimensions.second, stride);
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
