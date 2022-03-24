/*
 * Copyright (C) 2022 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

#include <matioCpp/ExogenousConversions.h>


matioCpp::Element<matioCpp::Logical> matioCpp::make_variable(const std::string& name, bool input)
{
    return matioCpp::Element<matioCpp::Logical>(name, input);
}

matioCpp::String matioCpp::make_variable(const std::string& name, const std::string& input)
{
    return matioCpp::String(name, input);
}

matioCpp::Vector<matioCpp::Logical> matioCpp::make_variable(const std::string& name, const std::vector<bool>& input)
{
    return matioCpp::Vector<matioCpp::Logical>(name, input);
}

matioCpp::CellArray matioCpp::make_variable(const std::string& name, const std::vector<std::string>& input)
{
    matioCpp::CellArray stringsArray(name, {input.size(), 1});
    for (size_t i = 0; i < input.size(); ++i)
    {
        stringsArray.setElement(i, make_variable(input[i], input[i]));
    }

    return stringsArray;
}
