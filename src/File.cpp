/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */


#include <matioCpp/File.h>
#include <time.h>
#include <matioCpp/Config.h>

class matioCpp::File::Impl
{
public:
    mat_t* mat_ptr{nullptr};
    matioCpp::FileMode fileMode{matioCpp::FileMode::ReadOnly};

    void close()
    {
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

        if (!newPtr)
        {
            mode = matioCpp::FileMode::ReadOnly;
        }
    }

    Impl()
    { }

    ~Impl()
    {
        close();
    }

    std::string isVariableValid(const matioCpp::Variable& input)
    {
        if (!input.isValid())
        {
            return "The input variable is not valid.";
        }

        const std::string& inputName = input.name();

        if (inputName.size() == 0)
        {
            return std::string("The input variable has an empty name.");
        }

        if (inputName.size() > 63)
        {
            return std::string("The input variable name is too long (maximum 63 char allowed)."); // Many Matlab versions have this limitation
        }

        if (!isalpha(inputName[0]))
        {
            return std::string("The first character of the variable name has to be a letter (Variable name = " + inputName + ").");
        }

        for (size_t i = 1; i < inputName.size(); ++i)
        {
            if (!isalnum(inputName[i]) && (inputName[i] != '_'))
            {
                return std::string("The variable name can contain only alphanumeric characters or underscores (Variable name = \"" + inputName + "\").");
            }
        }

        return std::string(); //Empty string, no error
    }
};

matioCpp::File::File()
    : m_pimpl(std::make_unique<Impl>())
{

}

matioCpp::File::File(const std::string &name, matioCpp::FileMode mode)
    : m_pimpl(std::make_unique<Impl>())
{
    open(name, mode);
}

matioCpp::File::File(matioCpp::File &&other)
{
    operator=(std::forward<matioCpp::File>(other));
}

matioCpp::File::~File()
{

}

void matioCpp::File::operator=(matioCpp::File &&other)
{
    m_pimpl = std::move(other.m_pimpl);
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

    std::string fileHeader = header;

    if (!fileHeader.size())
    {
        time_t rawtime;
        time(&rawtime);
        std::string timeString(ctime(&rawtime));
        size_t end = timeString.find_last_of('\n');
        timeString = timeString.substr(0,end);

        if (fileVer == mat_ft::MAT_FT_MAT73)
        {
            fileHeader = std::string("MATLAB 7.3 MAT-file, created by matioCpp v") + MATIOCPP_VER +
                         " via libmatio v" + std::to_string(MATIO_MAJOR_VERSION)
                         + "." + std::to_string(MATIO_MINOR_VERSION) + "." + std::to_string(MATIO_RELEASE_LEVEL) +
                         " on " + timeString + " HDF5 schema 0.5.";
        }
        else if (fileVer == mat_ft::MAT_FT_MAT5)
        {
            fileHeader = std::string("MATLAB 5.0 MAT-file, created by matioCpp v") + MATIOCPP_VER +
                         " via libmatio v" + std::to_string(MATIO_MAJOR_VERSION)
                         + "." + std::to_string(MATIO_MINOR_VERSION) + "." + std::to_string(MATIO_RELEASE_LEVEL) +
                         " on " + timeString + ".";
        }
        //MAT4 does not have a header
    }

    const char * matioHeader = fileHeader.size() ? fileHeader.c_str() : NULL;

    newFile.m_pimpl->reset(Mat_CreateVer(name.c_str(), matioHeader, fileVer), matioCpp::FileMode::ReadAndWrite);

    if (!newFile.isOpen())
    {
        std::cerr << "[ERROR][matioCpp::File::Create] Failed to open the file." <<std::endl;
    }

    return newFile;
}

bool matioCpp::File::Delete(const std::string &name)
{
    return std::remove(name.c_str()) == 0;
}

bool matioCpp::File::Exists(const std::string &name)
{
    matioCpp::File test(name, matioCpp::FileMode::ReadOnly);
    return test.isOpen();
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

#if MATIO_VERSION >= 1515
    return Mat_GetHeader(m_pimpl->mat_ptr);
#else
    std::cerr << "[ERROR][matioCpp::File::header] The file header can be retrieved only with matio >= 1.5.15" << std::endl;
    return "";
#endif
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

std::vector<std::string> matioCpp::File::variableNames() const
{
    std::vector<std::string> outputNames;
    if (isOpen())
    {
        size_t list_size;
        char** list = Mat_GetDir(m_pimpl->mat_ptr, &list_size);

        outputNames.resize(list_size);
        for (size_t i = 0; i < list_size; ++i)
        {
            outputNames[i] = list[i];
        }
    }

    return outputNames;
}

matioCpp::Variable matioCpp::File::read(const std::string &name) const
{
    if (!isOpen())
    {
        std::cerr << "[ERROR][matioCpp::File::read] The file is not open." <<std::endl;
        return matioCpp::Variable();
    }

#if defined(_MSC_VER) && MATIO_VERSION < 1519
    if (version() == matioCpp::FileVersion::MAT7_3)
    {
        std::cerr << "[ERROR][matioCpp::File::read] Reading to a 7.3 file on Windows with a matio version previous to 1.5.19 causes segfaults. The output will be an invalid Variable." << std::endl;
        return matioCpp::Variable();
    }
#endif

    matvar_t *matVar = Mat_VarRead(m_pimpl->mat_ptr, name.c_str());

    matioCpp::Variable output((matioCpp::SharedMatvar(matVar)));

    return output;
}

bool matioCpp::File::write(const Variable &variable, Compression compression)
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

    std::string error = m_pimpl->isVariableValid(variable);
    if (error.size() != 0)
    {
        std::cerr << "[ERROR][matioCpp::File::write] " << error << std::endl;
        return false;
    }

    SharedMatvar shallowCopy = SharedMatvar::GetMatvarShallowDuplicate(variable.toMatio()); // Shallow copy to remove const

    matio_compression matioCompression =
            (compression == matioCpp::Compression::zlib) ? matio_compression::MAT_COMPRESSION_ZLIB : matio_compression::MAT_COMPRESSION_NONE;

    if (version() == matioCpp::FileVersion::MAT4)
    {
        switch (variable.variableType())
        {
        case matioCpp::VariableType::Element:
            break;
        case matioCpp::VariableType::Vector:
            break;
        case matioCpp::VariableType::MultiDimensionalArray:
            if (variable.dimensions().size() > 2)
            {
                std::cerr << "[ERROR][matioCpp::File::write] A MAT4 version does not support arrays with number of dimensions greater than 2." << std::endl;
                return false;
            }
            break;
        default:
            std::cerr << "[ERROR][matioCpp::File::write] A MAT4 supports only element, vectors or matrices." << std::endl;
            return false;
        }

        matioCpp::ValueType valueType = variable.valueType();

        if ((valueType != matioCpp::ValueType::DOUBLE) && (valueType != matioCpp::ValueType::SINGLE) && (valueType != matioCpp::ValueType::LOGICAL)
                && (valueType != matioCpp::ValueType::UINT8) && (valueType != matioCpp::ValueType::INT32)
                && (valueType != matioCpp::ValueType::INT16) && (valueType != matioCpp::ValueType::UINT16))
        {
            std::cerr << "[ERROR][matioCpp::File::write] A MAT4 supports only variables of type LOGICAL, DOUBLE, SINGLE, UINT8, UINT16, INT16 and INT32." << std::endl;
            return false;
        }
    }

    bool success = Mat_VarWrite(m_pimpl->mat_ptr, shallowCopy.get(), matioCompression) == 0;

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

