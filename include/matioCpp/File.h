#ifndef MATIOCPP_FILE_H
#define MATIOCPP_FILE_H

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */


#include <matioCpp/ForwardDeclarations.h>
#include <matioCpp/Variable.h>

class matioCpp::File
{
    class Impl;

    std::unique_ptr<Impl> m_pimpl;

public:

    File();

    File(const File& other) = delete;

    File(File&& other);

    ~File();

    void close();

    bool open(const std::string& name, matioCpp::FileMode mode = matioCpp::FileMode::ReadAndWrite); //if the mode is ReadAndWrite, a file with the same name is created if not found

    static File Create(const std::string& name,
                       matioCpp::FileVersion version = matioCpp::FileVersion::Default,
                       const std::string& header = "");

    std::string name() const;

    std::string header() const;

    matioCpp::FileVersion version() const;

    matioCpp::FileMode mode() const;

    const std::vector<std::string>& variableNames() const;

    matioCpp::Variable read(const std::string& name) const;

    bool write(Variable &variable);

    bool isOpen() const;

};


#endif // MATIOCPP_FILE_H
