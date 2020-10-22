#ifndef MATIOCPP_ELEMENT_TPP
#define MATIOCPP_ELEMENT_TPP

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

template<typename T>
bool matioCpp::Element<T>::checkCompatibility(const matvar_t *inputPtr) const
{
    if (!inputPtr)
    {
        std::cerr << "[matioCpp::Element::checkCompatibility] The input pointer is null." << std::endl;
        return false;
    }

    matioCpp::VariableType outputVariableType = matioCpp::VariableType::Unsupported;
    matioCpp::ValueType outputValueType = matioCpp::ValueType::UNSUPPORTED;
    get_types_from_matvart(inputPtr, outputVariableType, outputValueType);

    if (outputVariableType != matioCpp::VariableType::Element)
    {
        std::cerr << "[matioCpp::Element::checkCompatibility] The input variable is not an Element." << std::endl;
        return false;
    }

    if (inputPtr->isComplex)
    {
        std::cerr << "[matioCpp::Element::checkCompatibility] Cannot copy a complex variable to a non-complex one." << std::endl;
        return false;
    }

    if (!matioCpp::is_convertible_to_primitive_type<T>(outputValueType))
    {
        std::string dataType = "";
        std::string classType = "";

        get_types_names_from_matvart(inputPtr, classType, dataType);

        std::cerr << "[matioCpp::Element::checkCompatibility] The input type is not convertible to " <<
            typeid(T).name() <<"." << std::endl <<
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
    T empty;
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
    T empty;
    size_t emptyDimensions[] = {1, 1};
    initializeVariable(name,
                       VariableType::Element,
                       matioCpp::get_type<T>::valueType(), emptyDimensions,
                       (void*)&empty);
}

template<typename T>
matioCpp::Element<T>::Element(const std::string &name, T inputValue)
{
    static_assert(!std::is_same<T, std::string>::value, "A string is not handled by matio. Use Vector<char> instead." );
    T empty = inputValue;
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
    if (!checkCompatibility(handler.get()))
    {
        assert(false);
        T empty;
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
matioCpp::Element<T> &matioCpp::Element<T>::operator=(T value)
{
    operator()() = value;
    return *this;
}

template<typename T>
bool matioCpp::Element<T>::setName(const std::string &newName)
{
    T oldValue = operator()();
    return initializeVariable(newName,
                              VariableType::Element,
                              valueType(), dimensions(),
                              (void*)&oldValue);
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
matioCpp::Element<T>::operator T() const
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
