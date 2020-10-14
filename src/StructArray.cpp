/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */


#include <matioCpp/StructArray.h>

bool matioCpp::StructArray::checkCompatibility(const matvar_t *inputPtr) const
{

}

matioCpp::StructArray::StructArray()
{

}

matioCpp::StructArray::StructArray(const std::string &name)
{

}

matioCpp::StructArray::StructArray(const std::string &name, const std::vector<matioCpp::StructArray::index_type> &dimensions)
{

}

matioCpp::StructArray::StructArray(const std::string &name, const std::vector<matioCpp::StructArray::index_type> &dimensions, const std::vector<matioCpp::Struct> &elements)
{

}

matioCpp::StructArray::StructArray(const matioCpp::StructArray &other)
{

}

matioCpp::StructArray::StructArray(matioCpp::StructArray &&other)
{

}

matioCpp::StructArray::StructArray(const MatvarHandler &handler)
{

}

matioCpp::StructArray::~StructArray()
{

}

matioCpp::StructArray &matioCpp::StructArray::operator=(const matioCpp::StructArray &other)
{

}

matioCpp::StructArray &matioCpp::StructArray::operator=(matioCpp::StructArray &&other)
{

}

bool matioCpp::StructArray::fromVectorOfStructs(const std::vector<matioCpp::StructArray::index_type> &dimensions, const std::vector<matioCpp::Struct> &elements)
{

}

matioCpp::StructArray::index_type matioCpp::StructArray::rawIndexFromIndices(const std::vector<matioCpp::StructArray::index_type> &el) const
{

}

bool matioCpp::StructArray::indicesFromRawIndex(size_t rawIndex, std::vector<matioCpp::StructArray::index_type> &el) const
{

}

bool matioCpp::StructArray::setName(const std::string &newName)
{

}

void matioCpp::StructArray::resize(const std::vector<matioCpp::StructArray::index_type> &newDimensions)
{

}

matioCpp::StructArray::index_type matioCpp::StructArray::numberOfElements() const
{

}

matioCpp::StructArray::index_type matioCpp::StructArray::numberOfFields() const
{

}

std::vector<std::string> matioCpp::StructArray::fields() const
{

}

bool matioCpp::StructArray::isFieldExisting(const std::string &field) const
{

}

size_t matioCpp::StructArray::getFieldIndex(const std::string &field) const
{

}

bool matioCpp::StructArray::addField(const std::string &newField)
{

}

bool matioCpp::StructArray::setElement(const std::vector<matioCpp::StructArray::index_type> &el, const matioCpp::Struct &newValue)
{

}

bool matioCpp::StructArray::setElement(matioCpp::StructArray::index_type el, const matioCpp::Struct &newValue)
{

}

matioCpp::Struct matioCpp::StructArray::operator()(const std::vector<matioCpp::StructArray::index_type> &el)
{

}

const matioCpp::Struct matioCpp::StructArray::operator()(const std::vector<matioCpp::StructArray::index_type> &el) const
{

}

matioCpp::Struct matioCpp::StructArray::operator()(matioCpp::StructArray::index_type el)
{

}

const matioCpp::Struct matioCpp::StructArray::operator()(matioCpp::StructArray::index_type el) const
{

}

matioCpp::Struct matioCpp::StructArray::operator[](const std::vector<matioCpp::StructArray::index_type> &el)
{

}

const matioCpp::Struct matioCpp::StructArray::operator[](const std::vector<matioCpp::StructArray::index_type> &el) const
{

}

matioCpp::Struct matioCpp::StructArray::operator[](matioCpp::StructArray::index_type el)
{

}

const matioCpp::Struct matioCpp::StructArray::operator[](matioCpp::StructArray::index_type el) const
{

}
