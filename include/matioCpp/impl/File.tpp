#ifndef MATIOCPP_FILE_TPP
#define MATIOCPP_FILE_TPP

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

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
