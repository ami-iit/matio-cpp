#ifndef MATIOCPP_FORWARDDECLARATIONS_H
#define MATIOCPP_FORWARDDECLARATIONS_H

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * This software may be modified and distributed under the terms of the
 * BSD-2-Clause license (https://opensource.org/licenses/BSD-2-Clause).
 */
#include <algorithm> // for lexicographical_compare
#include <array>     // for array
#include <cassert>
#include <cctype> // for isalpha, isalnum
#include <complex>
#include <cstddef>   // for ptrdiff_t, size_t, nullptr_t
#include <cstdio>  //for remove
#include <cstring> //for memcpy
#include <initializer_list>
#include <iostream>
#include <iterator>  // for reverse_iterator, distance, random_access_...
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits> // for enable_if_t, declval, is_convertible, inte...
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <matio.h>

#include <matioCpp/Config.h>

#ifdef _MSC_VER
#pragma warning(push)

// turn off some warnings that are noisy about our Expects statements
#pragma warning(disable : 4127) // conditional expression is constant
#pragma warning(disable : 4702) // unreachable code

// blanket turn off warnings from CppCoreCheck for now
// so people aren't annoyed by them when running the tool.
#pragma warning(disable : 26481 26482 26483 26485 26490 26491 26492 26493 26495)

#if _MSC_VER < 1910
#pragma push_macro("constexpr")
#define constexpr /*constexpr*/
#define MATIOCPP_USE_STATIC_CONSTEXPR_WORKAROUND

#endif                          // _MSC_VER < 1910
#else                           // _MSC_VER

// See if we have enough C++17 power to use a static constexpr data member
// without needing an out-of-line definition
#if !(defined(__cplusplus) && (__cplusplus >= 201703L))
#define MATIOCPP_USE_STATIC_CONSTEXPR_WORKAROUND
#endif // !(defined(__cplusplus) && (__cplusplus >= 201703L))

#endif                          // _MSC_VER

// constexpr workaround for SWIG
#ifdef SWIG
#define MATIOCPP_CONSTEXPR
#else
#define MATIOCPP_CONSTEXPR constexpr
#endif

namespace matioCpp
{

/**
 * dependent_false is a type-dependent expression that is always false. Please check
 * https://en.cppreference.com/w/cpp/language/if for further details.
 */
template <class T> struct dependent_false : std::false_type
{
};

/**
 * Utility metafunction to avoid compiler warnings about unused variables.
 */
template <typename... Args> inline void unused(Args&&...) {}

/**
 * @brief Define the type of variable
 */
enum class VariableType
{
    Element,
    Vector,
    MultiDimensionalArray,
    Struct,
    CellArray,
    StructArray,
    Unsupported
};

/**
 * @brief The list of types for an element of a certain variable type
 */
enum class ValueType
{
    INT8,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32,
    SINGLE,
    DOUBLE,
    INT64,
    UINT64,
    UTF8,
    UTF16,
    UTF32,
    STRING,
    LOGICAL,
    VARIABLE, //This is the case of composite containers, like Struct, CellArray, and Container
    UNSUPPORTED
};

/**
 * @brief The available modes with which a file can be opened.
 */
enum class FileMode
{
    ReadOnly,
    ReadAndWrite
};

/**
 * @brief The supported file versions
 */
enum class FileVersion
{
    Default,   /** @brief This is one of the following three depending on the matio installation. **/
    MAT4,      /** @brief Matlab version 4 file             */
    MAT5,      /** @brief Matlab version 5 file               */
    MAT7_3,    /** @brief Matlab version 7.3 file               */
    Undefined  /** @brief Undefined version                   */
};

/**
 * @brief Compression type when writing to a file
 */
enum class Compression
{
    None, /** @brief No compression. **/
    zlib /** @brief Use zlib compression. **/
};

/**
 * @brief The delete mode of matvar_t pointers.
 */
enum class DeleteMode
{
    DoNotDelete, /** The handler does not delete the pointer. **/
    ShallowDelete, /** The handler deletes the pointer but not the data. **/
    Delete /** The handler deletes the pointer. **/
};

/**
 * @brief Utility class to consider logical values.
 */
class Logical {
private:
    Logical() {}; //Logical is meant to be used only as type.
};

// [views.constants], constants
MATIOCPP_CONSTEXPR const std::ptrdiff_t dynamic_extent = -1;

template <class Container, bool IsConst>
class vector_iterator;

template <class ElementType, std::ptrdiff_t Extent = dynamic_extent>
class Span;

class MatvarHandler;

class SharedMatvar;

class WeakMatvar;

class Variable;

template<typename T>
class Element;

template<typename T>
class Vector;

using String = Vector<char>;

using String16 = Vector<char16_t>;

using String32 = Vector<char32_t>;

template<typename T>
class MultiDimensionalArray;

class CellArray;

class File;

class Struct;

class StructArray;

template <bool isConst>
class StructArrayElement;

}

#endif // MATIOCPP_FORWARDDECLARATIONS_H
