#ifndef MATIOCPP_STRUCTARRAYELEMENT_TPP
#define MATIOCPP_STRUCTARRAYELEMENT_TPP

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

template <bool isConst>
matioCpp::StructArrayElement<isConst>::StructArrayElement(index_type index, input_vector_type *array)
    : m_innerIndex(index)
    , m_array(array)
{
    assert(m_array);
}

template <bool isConst>
matioCpp::StructArrayElement<isConst>::StructArrayElement(const matioCpp::StructArrayElement<isConst> &other)
    : m_innerIndex(other.m_innerIndex)
    , m_array(other.m_array)
{
    assert(m_array);
}

template <bool isConst>
template<bool B, typename >
matioCpp::StructArrayElement<isConst>::StructArrayElement(const matioCpp::StructArrayElement<B> &other)
    : m_innerIndex(other.m_innerIndex)
    , m_array(other.m_array)
{
    static_assert (!B && isConst, "This constructor is meant to be used to obtain a const element from a non const one.");
    assert(m_array);
}

template <bool isConst>
matioCpp::StructArrayElement<isConst>::StructArrayElement(matioCpp::StructArrayElement<isConst> &&other)
    : m_innerIndex(other.m_innerIndex)
    , m_array(other.m_array)
{
    assert(m_array);
}

template <bool isConst>
template<bool B, typename >
matioCpp::StructArrayElement<isConst>::StructArrayElement(matioCpp::StructArrayElement<B> &&other)
    : m_innerIndex(other.m_innerIndex)
    , m_array(other.m_array)
{
    static_assert (!B && isConst, "This constructor is meant to be used to obtain a const element from a non const one.");
    assert(m_array);
}

template <bool isConst>
matioCpp::StructArrayElement<isConst> &matioCpp::StructArrayElement<isConst>::operator=(matioCpp::StructArrayElement<isConst> &&other)
{
    m_innerIndex = other.m_innerIndex;
    m_array = other.m_array;
    assert(m_array);
    return *this;
}

template <bool isConst>
template<bool B, typename >
matioCpp::StructArrayElement<isConst> &matioCpp::StructArrayElement<isConst>::operator=(matioCpp::StructArrayElement<B> &&other)
{
    static_assert (!B && isConst, "This operator is meant to be used to obtain a const element from a non const one.");

    m_innerIndex = other.m_innerIndex;
    m_array = other.m_array;
    assert(m_array);
    return *this;
}

template <bool isConst>
template<bool B, typename >
matioCpp::StructArrayElement<isConst> &matioCpp::StructArrayElement<isConst>::operator=(const matioCpp::Struct &other) const
{
    static_assert (!B && isConst, "This operator is meant to be used to obtain a const element from a non const one.");

    bool ok = m_array->setElement(m_innerIndex, other);
    assert(ok && "Failed to set the specified element to the input struct.");
    matioCpp::unused(ok);

    return *this;
}

template <bool isConst>
template<bool B, typename >
bool matioCpp::StructArrayElement<isConst>::fromVectorOfVariables(const std::vector<Variable> &elements) const
{
    static_assert ((B == isConst) && !B, "This method can be used only if the the element is not const.");

    char * const * arrayFields = m_array->getStructFields();

    if (m_array->numberOfFields() != elements.size())
    {
        std::cerr << "[ERROR][matioCpp::StructArrayElement::fromVectorOfVariables] The input vector is supposed to have size equal to the number of fields of the struct array." <<std::endl;
        return false;
    }

    for (size_t i = 0; i < m_array->numberOfFields(); ++i)
    {
        if (strcmp(arrayFields[i], elements[i].name().c_str()) != 0)
        {
            std::cerr << "[ERROR][matioCpp::StructArrayElement::operator=] The name " << elements[i].name().c_str() << " of the input vector of variables at position "
                      << std::to_string(i) << " is supposed to be " << arrayFields[i]
                      << ". Cannot insert in a struct array a new field in a single element." <<std::endl;
            return false;
        }

        bool ok = m_array->setStructField(i, elements[i], m_innerIndex);
        if (!ok)
        {
            std::cerr << "[ERROR][matioCpp::StructArrayElement::operator=] Failed to set field " << arrayFields[i] << "." <<std::endl;
            return false;
        }
    }

    return true;
}

template <bool isConst>
typename matioCpp::StructArrayElement<isConst>::index_type matioCpp::StructArrayElement<isConst>::numberOfFields() const
{
    return m_array->numberOfFields();
}

template <bool isConst>
std::vector<std::string> matioCpp::StructArrayElement<isConst>::fields() const
{
    return m_array->fields();
}

template <bool isConst>
bool matioCpp::StructArrayElement<isConst>::isFieldExisting(const std::string &field) const
{
    return m_array->isFieldExisting(field);
}

template <bool isConst>
size_t matioCpp::StructArrayElement<isConst>::getFieldIndex(const std::string &field) const
{
    return m_array->getFieldIndex(field);
}

template <bool isConst>
template<bool B, typename >
bool matioCpp::StructArrayElement<isConst>::setField(index_type index, const Variable &newValue) const
{
    static_assert ((B == isConst) && !B, "This method can be used only if the the element is not const.");

    assert(index < numberOfFields() && "The index is out of bounds.");
    if (!newValue.isValid())
    {
        std::cerr << "[ERROR][matioCpp::StructArrayElement::setField] The input variable is not valid." << std::endl;
        return false;
    }

    return m_array->setStructField(index, newValue, m_innerIndex);
}

template <bool isConst>
template<bool B, typename >
bool matioCpp::StructArrayElement<isConst>::setField(const std::string& field, const Variable &newValue) const
{
    static_assert ((B == isConst) && !B, "This method can be used only if the the element is not const.");

    size_t index = getFieldIndex(field);
    if (index == numberOfFields())
    {
        std::cerr << "[ERROR][matioCpp::StructArrayElement::setField] No field named " << field << "." << std::endl;
        return false;
    }

    return setField(index, newValue);
}

template <bool isConst>
template<bool B, typename >
bool matioCpp::StructArrayElement<isConst>::setField(const Variable &newValue) const
{
    static_assert ((B == isConst) && !B, "This method can be used only if the the element is not const.");

    return setField(newValue.name(), newValue);
}

template <bool isConst>
typename matioCpp::StructArrayElement<isConst>::output_struct_type matioCpp::StructArrayElement<isConst>::asStruct() const
{
    return m_array->getStructArrayElement(m_innerIndex);
}

template <bool isConst>
typename matioCpp::StructArrayElement<isConst>::output_variable_type matioCpp::StructArrayElement<isConst>::operator ()(index_type el) const
{
    assert(el < numberOfFields() && "The specified index is out of bounds");
    return m_array->getStructField(el, m_innerIndex);
}

template <bool isConst>
typename matioCpp::StructArrayElement<isConst>::output_variable_type matioCpp::StructArrayElement<isConst>::operator ()(const std::string &el) const
{
    size_t index = getFieldIndex(el);
    assert(index < numberOfFields() && "The specified field does not exist.");
    return m_array->getStructField(index, m_innerIndex);
}

template <bool isConst>
typename matioCpp::StructArrayElement<isConst>::output_variable_type matioCpp::StructArrayElement<isConst>::operator [](index_type el) const
{
    assert(el < numberOfFields() && "The specified index is out of bounds");
    return m_array->getStructField(el, m_innerIndex);
}

template <bool isConst>
typename matioCpp::StructArrayElement<isConst>::output_variable_type matioCpp::StructArrayElement<isConst>::operator [](const std::string &el) const
{
    size_t index = getFieldIndex(el);
    assert(index < numberOfFields() && "The specified field does not exist.");
    return m_array->getStructField(index, m_innerIndex);
}

#endif // MATIOCPP_STRUCTARRAYELEMENT_TPP
