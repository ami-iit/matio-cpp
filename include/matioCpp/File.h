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

    /**
     * @brief Default Constructor
     */
    File();

    /**
     * @brief Constructor opening the specified file
     * @param name The name of the file to open.
     * @param mode The mode with which to open the file. By default, both reading and writing are allowed.
     * @note If the mode is ReadAndWrite, a file with the same name is created if not found
     */
    File(const std::string& name, matioCpp::FileMode mode = matioCpp::FileMode::ReadAndWrite);

    /**
     * @brief Deleted copy constructor, to avoid confusion on whether the content has been copied or not
     */
    File(const File& other) = delete;

    /**
     * @brief Move constructor
     * @param other The other File from which the internal status has been taken.
     */
    File(File&& other);

    /**
     * @brief Destructor
     */
    ~File();

    /**
     * @brief Close the file
     */
    void close();

    /**
     * @brief Open the specified file
     * @param name The name of the file to open.
     * @param mode The mode with which to open the file. By default, both reading and writing are allowed.
     * @return false in case of failure (the file does not exist, the file exist but cannot be written, ..)
     */
    bool open(const std::string& name, matioCpp::FileMode mode = matioCpp::FileMode::ReadAndWrite);

    /**
     * @brief Create a new file (static)
     *
     * This is the static version of open, with the addition that it is possible to have additional settings.
     * @param name The name of the file to be openend.
     * @param version The MAT file version.
     * @param header This is an optional string that is added inside the MAT file. You can view the text in a MAT-file header using the cat command on the file on UNIX.
     * @return a newly created File object. The method isOpen() returnf false if something went wrong.
     */
    static File Create(const std::string& name,
                       matioCpp::FileVersion version = matioCpp::FileVersion::Default,
                       const std::string& header = "");

    /**
     * @brief Delete the specified file
     * @param name Name of the file to delete.
     * @return True if successfull, false otherwise (for example, in Windows the file will not be deleted if open).
     */
    static bool Delete(const std::string& name);

    /**
     * @brief The file name
     * @return The file name
     */
    std::string name() const;

    /**
     * @brief The file header
     *
     * This is an optional string that is added inside the MAT file. You can view the text in a MAT-file header using the cat command on the file on UNIX.
     * @warning This is available only if the matio version is >= 1.5.15
     *
     * @return The file header
     */
    std::string header() const;

    /**
     * @brief The MAT file version
     * @return The MAT file version
     */
    matioCpp::FileVersion version() const;

    /**
     * @brief The mode with which the file has been opened
     *
     * @return The file mode. If the file is not open, it returns ReadOnly.
     */
    matioCpp::FileMode mode() const;

    /**
     * @brief Get the list of variables in the file.
     * @return The list of variables in the file.
     */
    const std::vector<std::string>& variableNames() const;

    /**
     * @brief Read a variable given the name
     * @param name The name of the variable to be read
     * @return The desired Variable. The method isValid() would return false if something went wrong.
     * @note Modifying the output variable will not change the file.
     */
    matioCpp::Variable read(const std::string& name) const;

    /**
     * @brief Write a Variable to a file
     * @param variable The input variable.
     * @note The input variable is not const since matio needs a non-const matvar_t pointer when writing to a file.
     * @return True if successfull.
     */
    bool write(Variable &variable);

    /**
     * @brief Check if the file is open
     * @return True if open.
     */
    bool isOpen() const;

};


#endif // MATIOCPP_FILE_H
