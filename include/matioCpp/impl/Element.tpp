#ifndef MATIOCPP_ELEMENT_TPP
#define MATIOCPP_ELEMENT_TPP

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */

template<typename T>
bool matioCpp::Element<T>::checkCompatibility(const matvar_t* inputPtr, matioCpp::VariableType variableType, matioCpp::ValueType valueType) const
{

    if (variableType != matioCpp::VariableType::Element)
    {
        std::cerr << "[matioCpp::Element::checkCompatibility] The variable type is not compatible with an Element." << std::endl;
        return false;
    }

    if (inputPtr->isComplex)
    {
        std::cerr << "[matioCpp::Element::checkCompatibility] Cannot use a complex variable into a non-complex one." << std::endl;
        return false;
    }

    if (!matioCpp::is_convertible_to_primitive_type<T>(valueType))
    {
        std::string dataType = "";
        std::string classType = "";

        get_types_names_from_matvart(inputPtr, classType, dataType);

        std::cerr << "[matioCpp::Element::checkCompatibility] The value type is not convertible to " <<
            get_type<T>::toString() <<"." << std::endl <<
            "                                        Input class type: " << classType << std::endl <<
            "                                        Input data type: " << dataType << std::endl;
        return false;
    }
    return true;
}

template<typename T>
matioCpp::Element<T>::Element()
{
    static_assert(!std::is_same<T, std::string>::value, "A string is not handled by matio. Use Vector<char> instead." );
    typename matioCpp::Element<T>::element_type empty;

    if (std::is_same<T, char>::value) //If the type is char, the name corresponds to the content
    {
        empty = '\0';
    }
    size_t emptyDimensions[] = {1, 1};
    initializeVariable("unnamed_element",
                       VariableType::Element,
                       matioCpp::get_type<T>::valueType(), emptyDimensions,
                       (void*)&empty);
}

template<typename T>
matioCpp::Element<T>::Element(const std::string &name)
{
    static_assert(!std::is_same<T, std::string>::value, "A string is not handled by matio. Use Vector<char> instead." );
    typename matioCpp::Element<T>::element_type empty;
    size_t emptyDimensions[] = {1, 1};
    initializeVariable(name,
                       VariableType::Element,
                       matioCpp::get_type<T>::valueType(), emptyDimensions,
                       (void*)&empty);
}

template<typename T>
matioCpp::Element<T>::Element(const std::string &name, typename matioCpp::Element<T>::element_type inputValue)
{
    static_assert(!std::is_same<T, std::string>::value, "A string is not handled by matio. Use Vector<char> instead." );
    typename matioCpp::Element<T>::element_type empty = inputValue;
    size_t emptyDimensions[] = {1, 1};
    initializeVariable(name,
                       VariableType::Element,
                       matioCpp::get_type<T>::valueType(), emptyDimensions,
                       (void*)&empty);
}

template<typename T>
matioCpp::Element<T>::Element(const Element<T> &other)
{
    fromOther(other);
}

template<typename T>
matioCpp::Element<T>::Element(Element<T> &&other)
{
    fromOther(std::forward<matioCpp::Element<T>>(other));
}

template<typename T>
matioCpp::Element<T>::Element(const MatvarHandler &handler)
    : matioCpp::Variable(handler)
{
    if (!handler.get() || !checkCompatibility(handler.get(), handler.variableType(), handler.valueType()))
    {
        assert(false);
        typename matioCpp::Element<T>::element_type empty;
        size_t emptyDimensions[] = {1, 1};
        initializeVariable("unnamed_element",
                           VariableType::Element,
                           matioCpp::get_type<T>::valueType(), emptyDimensions,
                           (void*)&empty);
    }
}

template<typename T>
matioCpp::Element<T>::~Element()
{

}

template<typename T>
matioCpp::Element<T> &matioCpp::Element<T>::operator=(const Element<T> &other)
{
    fromOther(other);
    return *this;
}

template<typename T>
matioCpp::Element<T> &matioCpp::Element<T>::operator=(Element<T> &&other)
{
    fromOther(std::forward<matioCpp::Element<T>>(other));
    return *this;
}

template<typename T>
matioCpp::Element<T> &matioCpp::Element<T>::operator=(typename matioCpp::Element<T>::element_type value)
{
    operator()() = value;
    return *this;
}

template<typename T>
bool matioCpp::Element<T>::setName(const std::string &newName)
{
    return changeName(newName);
}

template<typename T>
typename matioCpp::Element<T>::reference matioCpp::Element<T>::operator()()
{
    return (((typename matioCpp::Element<T>::pointer)(toMatio()->data))[0]);
}

template<typename T>
typename matioCpp::Element<T>::value_type matioCpp::Element<T>::operator()() const
{
    return (((typename matioCpp::Element<T>::const_pointer)(toMatio()->data))[0]);
}

template<typename T>
matioCpp::Element<T>::operator typename matioCpp::Element<T>::element_type() const
{
    return (((typename matioCpp::Element<T>::const_pointer)(toMatio()->data))[0]);
}

template<typename T>
matioCpp::Element<T> matioCpp::Variable::asElement()
{
    return matioCpp::Element<T>(*m_handler);
}

template<typename T>
const matioCpp::Element<T> matioCpp::Variable::asElement() const
{
    return matioCpp::Element<T>(*m_handler);
}

#endif // MATIOCPP_ELEMENT_TPP
