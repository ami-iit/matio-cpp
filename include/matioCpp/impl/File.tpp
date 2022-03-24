/*
 * Copyright (C) 2022 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

#ifndef MATIOCPP_FILE_TPP
#define MATIOCPP_FILE_TPP

template<class input>
const input &matioCpp::File::getVariable(const input& it)
{
    return it;
}

template<class key, class input>
const input &matioCpp::File::getVariable(const std::pair<key, input>& it)
{
    return it.second;
}

template <class iterator>
bool matioCpp::File::write(iterator begin, iterator end, Compression compression)
{
    for (iterator it = begin; it != end; ++it)
    {
        if (!write(getVariable(*it), compression))
        {
            return false;
        }
    }

    return true;
}

#endif // MATIOCPP_FILE_TPP
