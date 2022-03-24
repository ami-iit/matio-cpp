/*
 * Copyright (C) 2022 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */
#ifndef MATIOCPP_EXOGENOUSCONVERSIONS_TPP
#define MATIOCPP_EXOGENOUSCONVERSIONS_TPP

template <class Vector, typename>
matioCpp::Vector<typename std::remove_cv_t<typename  matioCpp::SpanUtils::container_data<Vector>::type>> matioCpp::make_variable(const Vector& input, const std::string& name)
{
    using type = typename  matioCpp::SpanUtils::container_data<Vector>::type;
    return matioCpp::Vector<typename std::remove_cv_t<type>>(name, matioCpp::make_span(input)); //data is copied
}

inline matioCpp::String matioCpp::make_variable(const std::string& input, const std::string& name)
{
    return matioCpp::String(name, input);
}

inline matioCpp::Vector<matioCpp::Logical> matioCpp::make_variable(const std::vector<bool>& input, const std::string& name)
{
    return matioCpp::Vector<matioCpp::Logical>(name, input);
}

template<typename type, typename>
matioCpp::Element<type> matioCpp::make_variable(const type& input, const std::string& name)
{
    return matioCpp::Element<type>(name, input);
}

inline matioCpp::Element<matioCpp::Logical> matioCpp::make_variable(bool input, const std::string& name)
{
    return matioCpp::Element<matioCpp::Logical>(name, input);
}

inline matioCpp::CellArray matioCpp::make_variable(const std::vector<std::string>& input, const std::string& name)
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
matioCpp::Struct matioCpp::make_struct(iterator begin, iterator end, const std::string& name)
{
    matioCpp::Struct matioStruct(name);
    for (iterator it = begin; it != end; it++)
    {
        bool ok = matioStruct.setField(make_variable(it->second, it->first));
        unused(ok);
        assert(ok);
    }

    return matioStruct;
}

template<class iterator,
         typename std::enable_if_t<matioCpp::is_pair_iterator_string<iterator>::value>*>
matioCpp::CellArray matioCpp::make_cell_array(const iterator& begin, const iterator& end, const std::string& name)
{
    matioCpp::CellArray matioCellArray(name, {static_cast<size_t>(std::distance(begin, end)), 1});

    size_t index = 0;
    iterator it = begin;
    while (it != end)
    {
        bool ok = matioCellArray.setElement(index, make_variable(it->second, it->first));
        unused(ok);
        assert(ok);
        index++;
        it++;
    }

    return matioCellArray;
}

template<class iterator,
         typename std::enable_if_t<!matioCpp::is_pair<decltype(*std::declval<iterator>())>::value>*>
matioCpp::CellArray matioCpp::make_cell_array(const iterator& begin, const iterator& end, const std::string& name)
{
    matioCpp::CellArray matioCellArray(name, {static_cast<size_t>(std::distance(begin, end)), 1});

    size_t index = 0;
    iterator it = begin;
    while (it != end)
    {
        bool ok = matioCellArray.setElement(index, make_variable(*it, "imported_element_" + std::to_string(index)));
        unused(ok);
        assert(ok);
        index++;
        it++;
    }

    return matioCellArray;
}

#endif // MATIOCPP_EXOGENOUSCONVERSIONS_TPP
