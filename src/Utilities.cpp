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

bool matioCpp::get_matio_types(const matioCpp::VariableType &inputVariableType, const matioCpp::ValueType &inputValueType, matio_classes &outputMatioClasses, matio_types &outputMatioType)
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
        case matioCpp::ValueType::UNSUPPORTED:
            return false;
        default:
            outputMatioClasses = matio_classes::MAT_C_DOUBLE;
            outputMatioType = get_matio_value_type(inputValueType);
            break;
        }
    }
    else if (inputVariableType == VariableType::CellContainer)
    {
        outputMatioClasses = matio_classes::MAT_C_CELL;
        outputMatioType = get_matio_value_type(inputValueType);
    }
    else if (inputVariableType == VariableType::StructContainer)
    {
        outputMatioClasses = matio_classes::MAT_C_STRUCT;
        outputMatioType = get_matio_value_type(inputValueType);
    }
    else
    {
        return false;
    }

    return true;
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

bool matioCpp::get_types_from_matvart(const matvar_t *input, matioCpp::VariableType &outputVariableType, matioCpp::ValueType &outputValueType)
{
    if (!input)
    {
        return false;
    }

    switch (input->data_type)
    {
    case matio_types::MAT_T_INT8:
        outputValueType = matioCpp::ValueType::INT8;
        break;
    case matio_types::MAT_T_UINT8:
        outputValueType = matioCpp::ValueType::UINT8;
        break;
    case matio_types::MAT_T_INT16:
        outputValueType = matioCpp::ValueType::INT16;
        break;
    case matio_types::MAT_T_UINT16:
        outputValueType = matioCpp::ValueType::UINT16;
        break;
    case matio_types::MAT_T_INT32:
        outputValueType = matioCpp::ValueType::INT32;
        break;
    case matio_types::MAT_T_UINT32:
        outputValueType = matioCpp::ValueType::UINT32;
        break;
    case matio_types::MAT_T_SINGLE:
        outputValueType = matioCpp::ValueType::SINGLE;
        break;
    case matio_types::MAT_T_DOUBLE:
        outputValueType = matioCpp::ValueType::DOUBLE;
        break;
    case matio_types::MAT_T_INT64:
        outputValueType = matioCpp::ValueType::INT64;
        break;
    case matio_types::MAT_T_UINT64:
        outputValueType = matioCpp::ValueType::UINT64;
        break;
    case matio_types::MAT_T_UTF8:
        outputValueType = matioCpp::ValueType::UTF8;
        break;
    case matio_types::MAT_T_UTF16:
        outputValueType = matioCpp::ValueType::UTF16;
        break;
    case matio_types::MAT_T_UTF32:
        outputValueType = matioCpp::ValueType::UTF32;
        break;
    case matio_types::MAT_T_STRING:
        outputValueType = matioCpp::ValueType::STRING;
        break;
    case matio_types::MAT_T_CELL:
        outputValueType = matioCpp::ValueType::CELL;
        break;
    case matio_types::MAT_T_STRUCT:
        outputValueType = matioCpp::ValueType::STRUCT;
        break;
    case matio_types::MAT_T_ARRAY:
        outputValueType = matioCpp::ValueType::VECTOR;
        break;
    case matio_types::MAT_T_MATRIX:
        outputValueType = matioCpp::ValueType::MULTI_DIMENSIONAL_ARRAY;
        break;
    case matio_types::MAT_T_COMPRESSED:
    case matio_types::MAT_T_FUNCTION:
    case matio_types::MAT_T_UNKNOWN:
        outputValueType = matioCpp::ValueType::UNSUPPORTED;
        break;
    }

    if (input->class_type == matio_classes::MAT_C_CELL || outputValueType == matioCpp::ValueType::CELL)
    {
        outputVariableType = matioCpp::VariableType::CellContainer;
    }
    else if (input->class_type == matio_classes::MAT_C_STRUCT || outputValueType == matioCpp::ValueType::STRUCT)
    {
        outputVariableType = matioCpp::VariableType::StructContainer;
    }
    else if ((input->class_type == matio_classes::MAT_C_OBJECT) ||
             (input->class_type == matio_classes::MAT_C_SPARSE) ||
             (input->class_type == matio_classes::MAT_C_FUNCTION) ||
             (input->class_type == matio_classes::MAT_C_OPAQUE) ||
             (outputValueType == matioCpp::ValueType::CELL) ||
             (outputValueType == matioCpp::ValueType::STRUCT) ||
             (outputValueType == matioCpp::ValueType::VECTOR) ||
             (outputValueType == matioCpp::ValueType::MULTI_DIMENSIONAL_ARRAY) ||
             (outputValueType == matioCpp::ValueType::UNSUPPORTED) ||
             (input->compression != matio_compression::MAT_COMPRESSION_NONE) ||
             (input->rank < 2)) //Matio requires the rank to be at least 2
    {
        outputVariableType = matioCpp::VariableType::Unsupported;
        return true;
    }
    else if ((input->rank == 2) && ((input->dims[0] == 1) || (input->dims[1] == 1)))
    {
        outputVariableType = matioCpp::VariableType::Vector;
    }
    else
    {
        outputVariableType = matioCpp::VariableType::MultiDimensionalArray;
    }

    return true;
}
