/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */


#include <matioCpp/File.h>

class matioCpp::File::Impl
{
public:
    mat_t* mat_ptr{nullptr};
    std::vector<std::string> variableNames;
    matioCpp::FileMode fileMode{matioCpp::FileMode::ReadOnly};

    void close()
    {
        variableNames.clear();
        fileMode = matioCpp::FileMode::ReadOnly;
        freePtr();
    }

    void freePtr()
    {
        if (mat_ptr)
        {
            Mat_Close(mat_ptr);
            mat_ptr = nullptr;
        }
    }

    void reset(mat_t* newPtr, matioCpp::FileMode mode)
    {
        freePtr();
        mat_ptr = newPtr;
        fileMode = mode;

        if (newPtr)
        {
            size_t list_size;
            char** list = Mat_GetDir(newPtr, &list_size);

            variableNames.resize(list_size);
            for (size_t i = 0; i < list_size; ++i)
            {
                variableNames[i] = list[i];
            }
        }
        else
        {
            variableNames.clear();
            mode = matioCpp::FileMode::ReadOnly;
        }
    }

    Impl()
    { }

    ~Impl()
    {
        close();
    }
};

matioCpp::File::File()
    : m_pimpl(std::make_unique<Impl>())
{

}

matioCpp::File::File(matioCpp::File &&other)
{
    m_pimpl = std::move(other.m_pimpl);
}

matioCpp::File::~File()
{

}

void matioCpp::File::close()
{
    m_pimpl->close();
}

bool matioCpp::File::open(const std::string &name, matioCpp::FileMode mode)
{
    int matio_mode = mode == matioCpp::FileMode::ReadOnly ? mat_acc::MAT_ACC_RDONLY : mat_acc::MAT_ACC_RDWR;
    m_pimpl->reset(Mat_Open(name.c_str(), matio_mode), mode);
    return isOpen();
}

matioCpp::File matioCpp::File::Create(const std::string &name, matioCpp::FileVersion version, const std::string &header)
{
    matioCpp::File newFile;

    if (version == matioCpp::FileVersion::Undefined)
    {
        std::cerr << "[ERROR][matioCpp::File::Create] Cannot use Undefined as input version type." <<std::endl;
        return newFile;
    }

    mat_ft fileVer;

    switch (version)
    {
    case matioCpp::FileVersion::MAT4:
        fileVer = mat_ft::MAT_FT_MAT4;
        break;
    case matioCpp::FileVersion::MAT5:
        fileVer = mat_ft::MAT_FT_MAT5;
        break;
    case matioCpp::FileVersion::MAT7_3:
        fileVer = mat_ft::MAT_FT_MAT73;
        break;
    default:
        fileVer = MAT_FT_DEFAULT;
        break;
    }

    const char * matioHeader = header.size() ? header.c_str() : NULL;

    newFile.m_pimpl->reset(Mat_CreateVer(name.c_str(), matioHeader, fileVer), matioCpp::FileMode::ReadAndWrite);

    if (!newFile.isOpen())
    {
        std::cerr << "[ERROR][matioCpp::File::Create] Failed to open the file." <<std::endl;
    }

    return newFile;
}

std::string matioCpp::File::name() const
{
    if (!isOpen())
    {
        return "";
    }

    return Mat_GetFilename(m_pimpl->mat_ptr);
}

std::string matioCpp::File::header() const
{
    if (!isOpen())
    {
        return "";
    }

    return Mat_GetHeader(m_pimpl->mat_ptr);
}

matioCpp::FileVersion matioCpp::File::version() const
{
    if (!isOpen())
    {
        return matioCpp::FileVersion::Undefined;
    }

    switch (Mat_GetVersion(m_pimpl->mat_ptr))
    {
    case mat_ft::MAT_FT_MAT4:
        return matioCpp::FileVersion::MAT4;

    case mat_ft::MAT_FT_MAT5:
        return matioCpp::FileVersion::MAT5;

    case mat_ft::MAT_FT_MAT73:
        return matioCpp::FileVersion::MAT7_3;

    default:
        return matioCpp::FileVersion::Undefined;
    }
}

matioCpp::FileMode matioCpp::File::mode() const
{
    return m_pimpl->fileMode;
}

const std::vector<std::string> &matioCpp::File::variableNames() const
{
    return m_pimpl->variableNames;
}

matioCpp::Variable matioCpp::File::read(const std::string &name) const
{
    if (!isOpen())
    {
        std::cerr << "[ERROR][matioCpp::File::read] The file is not open." <<std::endl;
        return matioCpp::Variable();
    }

    matvar_t *matVar = Mat_VarRead(m_pimpl->mat_ptr, name.c_str());

    matioCpp::Variable output(matVar);

    Mat_VarFree(matVar); //The content of matvar has been deep copied in the Variable, thus we have to free manually the output of Mat_VarRead.

    return output;
}

bool matioCpp::File::write(matioCpp::Variable &variable)
{
    if (!isOpen())
    {
        std::cerr << "[ERROR][matioCpp::File::write] The file is not open." <<std::endl;
        return false;
    }

    if (mode() != matioCpp::FileMode::ReadAndWrite)
    {
        std::cerr << "[ERROR][matioCpp::File::write] The file cannot be written." <<std::endl;
        return false;
    }

    if (!variable.isValid())
    {
        std::cerr << "[ERROR][matioCpp::File::write] The input variable is not valid." <<std::endl;
        return false;
    }
    bool success = Mat_VarWrite(m_pimpl->mat_ptr, variable.toMatio(), matio_compression::MAT_COMPRESSION_NONE) == 0;

    if (!success)
    {
        std::cerr << "[ERROR][matioCpp::File::write] Failed to write the variable to the file." <<std::endl;
        return false;
    }

    return true;
}

bool matioCpp::File::isOpen() const
{
    return m_pimpl->mat_ptr;
}


