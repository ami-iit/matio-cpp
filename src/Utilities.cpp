/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

#include <matioCpp/Utilities.h>

void matioCpp::get_matio_types(const matioCpp::VariableType &inputVariableType, const matioCpp::ValueType &inputValueType, matio_classes &outputMatioClasses, matio_types &outputMatioType)
{
    if (inputVariableType == VariableType::Vector || inputVariableType == VariableType::MultiDimensionalArray)
    {
        switch (inputValueType)
        {
        case matioCpp::ValueType::INT8:
            outputMatioClasses = matio_classes::MAT_C_INT8;
            outputMatioType =  matio_types::MAT_T_INT8;
            break;
        case matioCpp::ValueType::UINT8:
            outputMatioClasses = matio_classes::MAT_C_UINT8;
            outputMatioType =  matio_types::MAT_T_UINT8;
            break;
        case matioCpp::ValueType::INT16:
            outputMatioClasses = matio_classes::MAT_C_INT16;
            outputMatioType =  matio_types::MAT_T_INT16;
            break;
        case matioCpp::ValueType::UINT16:
            outputMatioClasses = matio_classes::MAT_C_UINT16;
            outputMatioType =  matio_types::MAT_T_UINT16;
            break;
        case matioCpp::ValueType::INT32:
            outputMatioClasses = matio_classes::MAT_C_INT32;
            outputMatioType =  matio_types::MAT_T_INT32;
            break;
        case matioCpp::ValueType::UINT32:
            outputMatioClasses = matio_classes::MAT_C_UINT32;
            outputMatioType =  matio_types::MAT_T_UINT32;
            break;
        case matioCpp::ValueType::SINGLE:
            outputMatioClasses = matio_classes::MAT_C_SINGLE;
            outputMatioType =  matio_types::MAT_T_SINGLE;
            break;
        case matioCpp::ValueType::DOUBLE:
            outputMatioClasses = matio_classes::MAT_C_DOUBLE;
            outputMatioType =  matio_types::MAT_T_DOUBLE;
            break;
        case matioCpp::ValueType::INT64:
            outputMatioClasses = matio_classes::MAT_C_INT64;
            outputMatioType =  matio_types::MAT_T_INT64;
            break;
        case matioCpp::ValueType::UINT64:
            outputMatioClasses = matio_classes::MAT_C_UINT64;
            outputMatioType =  matio_types::MAT_T_UINT64;
            break;
        default:
            outputMatioClasses = matio_classes::MAT_C_DOUBLE;
            outputMatioType = get_matio_value_type(inputValueType);
            break;
        }
    }
}

matio_types matioCpp::get_matio_value_type(const matioCpp::ValueType &inputValueType)
{
    switch (inputValueType)
    {
    case matioCpp::ValueType::INT8:
        return matio_types::MAT_T_INT8;
    case matioCpp::ValueType::UINT8:
        return matio_types::MAT_T_UINT8;
    case matioCpp::ValueType::INT16:
        return matio_types::MAT_T_INT16;
    case matioCpp::ValueType::UINT16:
        return matio_types::MAT_T_UINT16;
    case matioCpp::ValueType::INT32:
        return matio_types::MAT_T_INT32;
    case matioCpp::ValueType::UINT32:
        return matio_types::MAT_T_UINT32;
    case matioCpp::ValueType::SINGLE:
        return matio_types::MAT_T_SINGLE;
    case matioCpp::ValueType::DOUBLE:
        return matio_types::MAT_T_DOUBLE;
    case matioCpp::ValueType::INT64:
        return matio_types::MAT_T_INT64;
    case matioCpp::ValueType::UINT64:
        return matio_types::MAT_T_UINT64;
    case matioCpp::ValueType::UTF8:
        return matio_types::MAT_T_UTF8;
    case matioCpp::ValueType::UTF16:
        return matio_types::MAT_T_UTF16;
    case matioCpp::ValueType::UTF32:
        return matio_types::MAT_T_UTF32;
    case matioCpp::ValueType::STRING:
        return matio_types::MAT_T_STRING;
    case matioCpp::ValueType::CELL:
        return matio_types::MAT_T_CELL;
    case matioCpp::ValueType::STRUCT:
        return matio_types::MAT_T_STRUCT;
    default:
        return matio_types::MAT_T_DOUBLE;
    }
}
