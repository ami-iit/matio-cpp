/*
 * Copyright (C) 2022 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */
#ifndef MATIOCPP_EXOGENOUSCONVERSIONS_TPP
#define MATIOCPP_EXOGENOUSCONVERSIONS_TPP

template <class Vector, typename>
inline matioCpp::Vector<typename std::remove_cv_t<typename  matioCpp::SpanUtils::container_data<Vector>::type>> matioCpp::make_variable(const std::string& name, const Vector& input)
{
    using type = typename  matioCpp::SpanUtils::container_data<Vector>::type;
    return matioCpp::Vector<typename std::remove_cv_t<type>>(name, matioCpp::make_span(input)); //data is copied
}

inline matioCpp::String matioCpp::make_variable(const std::string& name, const std::string& input)
{
    return matioCpp::String(name, input);
}

inline matioCpp::Vector<matioCpp::Logical> matioCpp::make_variable(const std::string& name, const std::vector<bool>& input)
{
    return matioCpp::Vector<matioCpp::Logical>(name, input);
}

template<typename type, typename>
inline matioCpp::Element<type> matioCpp::make_variable(const std::string& name, const type& input)
{
    return matioCpp::Element<type>(name, input);
}

inline matioCpp::Element<matioCpp::Logical> matioCpp::make_variable(const std::string& name, bool input)
{
    return matioCpp::Element<matioCpp::Logical>(name, input);
}

inline matioCpp::CellArray matioCpp::make_variable(const std::string& name, const std::vector<std::string>& input)
{
    matioCpp::CellArray stringsArray(name, {input.size(), 1});
    for (size_t i = 0; i < input.size(); ++i)
    {
        stringsArray.setElement(i, make_variable(input[i], input[i]));
    }

    return stringsArray;
}

template<class iterator,
          typename>
inline matioCpp::Struct matioCpp::make_struct(const std::string& name, iterator begin, iterator end)
{
    matioCpp::Struct matioStruct(name);
    for (iterator it = begin; it != end; it++)
    {
        bool ok = matioStruct.setField(make_variable(it->first, it->second));
        unused(ok);
        assert(ok);
    }

    return matioStruct;
}

template<class iterator,
         typename std::enable_if_t<matioCpp::is_pair_iterator_string<iterator>::value>*>
inline matioCpp::CellArray matioCpp::make_cell_array(const std::string& name, const iterator& begin, const iterator& end)
{
    matioCpp::CellArray matioCellArray(name, {static_cast<size_t>(std::distance(begin, end)), 1});

    size_t index = 0;
    iterator it = begin;
    while (it != end)
    {
        bool ok = matioCellArray.setElement(index, make_variable(it->first, it->second));
        unused(ok);
        assert(ok);
        index++;
        it++;
    }

    return matioCellArray;
}

template<class iterator,
         typename std::enable_if_t<!matioCpp::is_pair<decltype(*std::declval<iterator>())>::value>*>
inline matioCpp::CellArray matioCpp::make_cell_array(const std::string& name, const iterator& begin, const iterator& end)
{
    matioCpp::CellArray matioCellArray(name, {static_cast<size_t>(std::distance(begin, end)), 1});

    size_t index = 0;
    iterator it = begin;
    while (it != end)
    {
        bool ok = matioCellArray.setElement(index, make_variable("imported_element_" + std::to_string(index), *it));
        unused(ok);
        assert(ok);
        index++;
        it++;
    }

    return matioCellArray;
}

#endif // MATIOCPP_EXOGENOUSCONVERSIONS_TPP
