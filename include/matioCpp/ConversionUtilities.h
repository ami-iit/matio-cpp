#ifndef MATIOCPP_UTILITIES_H
#define MATIOCPP_UTILITIES_H
/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
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
 * @brief Get the type names from the input pointer
 * @param input The input pointer
 * @param classType The class type
 * @param dataType The data type
 */
bool get_types_names_from_matvart(const matvar_t* input, std::string &classType, std::string &dataType);

/**
 * @brief Utility metafunction to get the ValueType from a given primitive type.
 */
template <typename Tp> struct get_type
{
    static_assert (matioCpp::dependent_false<Tp>::value, "Unsupported type.");
    static_assert (!std::is_same<Tp, bool>::value, "Use matioCpp::Logical instead of bool.");
};

#ifdef __APPLE__
using size_t_type = unsigned long;
#else
using size_t_type = uint64_t;
#endif

// specializations
template <> struct get_type<int8_t>       { using type = int8_t;      static inline ValueType valueType(){return ValueType::INT8;};     static inline std::string toString(){return "int8_t"            ;};};
template <> struct get_type<uint8_t>      { using type = uint8_t;     static inline ValueType valueType(){return ValueType::UINT8;};    static inline std::string toString(){return "uint8_t"           ;};};
template <> struct get_type<int16_t>      { using type = int16_t;     static inline ValueType valueType(){return ValueType::INT16;};    static inline std::string toString(){return "int16_t"           ;};};
template <> struct get_type<uint16_t>     { using type = uint16_t;    static inline ValueType valueType(){return ValueType::UINT16;};   static inline std::string toString(){return "uint16_t"          ;};};
template <> struct get_type<int32_t>      { using type = int32_t;     static inline ValueType valueType(){return ValueType::INT32;};    static inline std::string toString(){return "int32_t"           ;};};
template <> struct get_type<uint32_t>     { using type = uint32_t;    static inline ValueType valueType(){return ValueType::UINT32;};   static inline std::string toString(){return "uint32_t"          ;};};
template <> struct get_type<float>        { using type = float;       static inline ValueType valueType(){return ValueType::SINGLE;};   static inline std::string toString(){return "float"             ;};};
template <> struct get_type<double>       { using type = double;      static inline ValueType valueType(){return ValueType::DOUBLE;};   static inline std::string toString(){return "double"            ;};};
template <> struct get_type<int64_t>      { using type = int64_t;     static inline ValueType valueType(){return ValueType::INT64;};    static inline std::string toString(){return "int64_t"           ;};};
template <> struct get_type<size_t_type>  { using type = size_t_type; static inline ValueType valueType(){return ValueType::UINT64;};   static inline std::string toString(){return "size_t"            ;};};
template <> struct get_type<char>         { using type = char;        static inline ValueType valueType(){return ValueType::UTF8;};     static inline std::string toString(){return "char"              ;};};
template <> struct get_type<char16_t>     { using type = char16_t;    static inline ValueType valueType(){return ValueType::UTF16;};    static inline std::string toString(){return "char16_t"          ;};};
template <> struct get_type<char32_t>     { using type = char32_t;    static inline ValueType valueType(){return ValueType::UTF32;};    static inline std::string toString(){return "char32_t"          ;};};
template <> struct get_type<Logical>      { using type = uint8_t;     static inline ValueType valueType(){return ValueType::LOGICAL;};  static inline std::string toString(){return "matioCpp::Logical" ;};};

/**
 * @brief Utility meta-function to check if a type is compatible with a std::string
 */
template <typename T>
struct is_string_compatible : std::false_type { };
template<> struct is_string_compatible<char> : std::true_type { };
template<> struct is_string_compatible<uint8_t> : std::true_type { };

/**
 * @brief Utility meta-function to check if a type is compatible with a std::u16string
 */
template <typename T>
struct is_string16_compatible : std::false_type { };
template<> struct is_string16_compatible<char16_t> : std::true_type { };
template<> struct is_string16_compatible<uint16_t> : std::true_type { };

/**
 * @brief Utility meta-function to check if a type is compatible with a std::u32string
 */
template <typename T>
struct is_string32_compatible : std::false_type { };
template<> struct is_string32_compatible<char32_t> : std::true_type { };
template<> struct is_string32_compatible<uint32_t> : std::true_type { };

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
        return (std::is_same<T, uint8_t>::value || std::is_same<T, char>::value);
        break;
    case matioCpp::ValueType::INT16:
        return std::is_same<T, int16_t>::value;
        break;
    case matioCpp::ValueType::UINT16:
        return (std::is_same<T, uint16_t>::value || std::is_same<T, char16_t>::value);
        break;
    case matioCpp::ValueType::INT32:
        return std::is_same<T, int32_t>::value;
        break;
    case matioCpp::ValueType::UINT32:
        return (std::is_same<T, uint32_t>::value || std::is_same<T, char32_t>::value);
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
        return std::is_same<T, size_t_type>::value;
        break;
    case matioCpp::ValueType::UTF8:
        return (std::is_same<T, char>::value || std::is_same<T, uint8_t>::value);
        break;
    case matioCpp::ValueType::UTF16:
        return (std::is_same<T, char16_t>::value || std::is_same<T, uint16_t>::value);
        break;
    case matioCpp::ValueType::UTF32:
        return (std::is_same<T, char32_t>::value || std::is_same<T, uint32_t>::value);
        break;
    case matioCpp::ValueType::LOGICAL:
        return std::is_same<T, matioCpp::Logical>::value;
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
