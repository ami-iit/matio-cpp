#ifndef MATIOCPP_UTILITIES_H
#define MATIOCPP_UTILITIES_H
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

namespace matioCpp {

/**
 * @brief Get both the matio type and class from the input VariableType and ValueType
 * @param inputVariableType The input VariableType.
 * @param inputValueType The input ValueType.
 * @param outputMatioClasses The corresponding matio class.
 * @param outputMatioType The corresponding matio type
 * @return True if a conversion was possible.
 */
bool get_matio_types(const VariableType& inputVariableType, const ValueType& inputValueType, matio_classes& outputMatioClasses, matio_types& outputMatioType);

/**
 * @brief Get the VariableType and the ValueType from a matvar_t pointer.
 * @param input The matvar_t pointer.
 * @param outputVariableType The output VariableType.
 * @param outputValueType The output ValueType.
 * @return False in case a conversion was not possible or the input pointere is null. True in case of success.
 */
bool get_types_from_matvart(const matvar_t* input, VariableType& outputVariableType, ValueType &outputValueType);

/**
 * @brief Utility metafunction to get the ValueType from a given primitive type.
 */
template <typename Tp> struct get_type;
// specializations
template <> struct get_type<int8_t>   { using type = int8_t;   static constexpr ValueType valueType = ValueType::INT8; };
template <> struct get_type<uint8_t>  { using type = uint8_t;  static constexpr ValueType valueType = ValueType::UINT8; };
template <> struct get_type<int16_t>  { using type =  int16_t; static constexpr ValueType valueType = ValueType::INT16; };
template <> struct get_type<uint16_t> { using type = uint16_t; static constexpr ValueType valueType = ValueType::UINT16; };
template <> struct get_type<int32_t>  { using type = int32_t;  static constexpr ValueType valueType = ValueType::INT32; };
template <> struct get_type<uint32_t> { using type = uint32_t; static constexpr ValueType valueType = ValueType::UINT32; };
template <> struct get_type<float>    { using type = float;    static constexpr ValueType valueType = ValueType::SINGLE; };
template <> struct get_type<double>   { using type = double;   static constexpr ValueType valueType = ValueType::DOUBLE; };
template <> struct get_type<int64_t>  { using type = int64_t;  static constexpr ValueType valueType = ValueType::INT64; };
template <> struct get_type<char> { using type = char; static constexpr ValueType valueType = ValueType::UTF8; };
template <> struct get_type<char16_t> { using type = char16_t; static constexpr ValueType valueType = ValueType::UTF16; };
template <> struct get_type<char32_t> { using type = char32_t; static constexpr ValueType valueType = ValueType::UTF32; };

/**
 * @brief Utility function to check if certain ValueType can be converted to a primitive type T.
 * @param type The input ValueType to test.
 * @return True in case it is possible to convert the input type to the primitive type T.
 */
template <typename T>
bool is_convertible_to_primitive_type(matioCpp::ValueType type)
{
    switch (type)
    {
    case matioCpp::ValueType::INT8:
        return std::is_same<T, int8_t>::value;
        break;
    case matioCpp::ValueType::UINT8:
        return std::is_same<T, uint8_t>::value;
        break;
    case matioCpp::ValueType::INT16:
        return std::is_same<T, int16_t>::value;
        break;
    case matioCpp::ValueType::UINT16:
        return std::is_same<T, uint16_t>::value;
        break;
    case matioCpp::ValueType::INT32:
        return std::is_same<T, int32_t>::value;
        break;
    case matioCpp::ValueType::UINT32:
        return std::is_same<T, uint32_t>::value;
        break;
    case matioCpp::ValueType::SINGLE:
        return std::is_same<T, float>::value;
        break;
    case matioCpp::ValueType::DOUBLE:
        return std::is_same<T, double>::value;
        break;
    case matioCpp::ValueType::INT64:
        return std::is_same<T, int64_t>::value;
        break;
    case matioCpp::ValueType::UINT64:
        return std::is_same<T, uint64_t>::value;
        break;
    case matioCpp::ValueType::UTF8:
        return std::is_same<T, char>::value;
        break;
    case matioCpp::ValueType::UTF16:
        return std::is_same<T, char16_t>::value;
        break;
    case matioCpp::ValueType::UTF32:
        return std::is_same<T, char32_t>::value;
        break;
    case matioCpp::ValueType::STRING:
    case matioCpp::ValueType::VARIABLE:
    case matioCpp::ValueType::UNSUPPORTED:
        return false;
    }
    return false;
}


}


#endif //MATIOCPP_UTILITIES_H
