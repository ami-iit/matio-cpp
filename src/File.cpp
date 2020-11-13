/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */


#include <matioCpp/File.h>

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

    const char * matioHeader = header.size() ? header.c_str() : NULL;

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

    matvar_t *matVar = Mat_VarRead(m_pimpl->mat_ptr, name.c_str());

    matioCpp::Variable output((matioCpp::SharedMatvar(matVar)));

    return output;
}

bool matioCpp::File::write(const Variable &variable)
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

    bool success = Mat_VarWrite(m_pimpl->mat_ptr, shallowCopy.get(), matio_compression::MAT_COMPRESSION_NONE) == 0;

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

