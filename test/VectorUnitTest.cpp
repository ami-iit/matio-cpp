/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

#include <catch2/catch.hpp>
#include <vector>
#include <matioCpp/matioCpp.h>

void checkSameDimensions(matioCpp::Span<const size_t> a, matioCpp::Span<const size_t> b)
{
    REQUIRE(a.size() == b.size());

    for (size_t i = 0; i < static_cast<size_t>(a.size()); ++i)
    {
        REQUIRE(a[i] == b[i]);
    }
}

void checkVariable(const matioCpp::Variable& var,
                   const std::string& name,
                   matioCpp::VariableType type,
                   matioCpp::ValueType value,
                   bool complex,
                   matioCpp::Span<const size_t> dimensions)
{
    REQUIRE(var.name() == name);
    REQUIRE(var.variableType() == type);
    REQUIRE(var.valueType() == value);
    REQUIRE(var.isComplex() == complex);
    REQUIRE(var.dimensions().size() == dimensions.size());
    checkSameDimensions(dimensions, var.dimensions());
}

void checkVariable(const matioCpp::Variable& var,
                   const std::string& name,
                   matioCpp::VariableType type,
                   matioCpp::ValueType value,
                   bool complex,
                   const std::vector<size_t>& dimensions)
{
    checkVariable(var, name, type, value, complex, matioCpp::make_span(dimensions));
}

void REQUIRE_TRUE(bool value)
{
    REQUIRE(value);
}

void checkSameVariable(const matioCpp::Variable& a, const matioCpp::Variable& b)
{
    checkVariable(a, b.name(), b.variableType(), b.valueType(), b.isComplex(), b.dimensions());
}

template <typename T>
void checkSameVector(const matioCpp::Vector<T>& a, const matioCpp::Vector<T>& b, double precision = 1e-15)
{
    checkVariable(a, b.name(), b.variableType(), b.valueType(), b.isComplex(), b.dimensions());

    for (size_t i = 0; i < a.size(); ++i)
    {
        REQUIRE(std::abs(a(i) - b(i)) < precision);
    }
}

template <typename T>
void checkSameVector(const T& a, const T& b, double precision = 1e-15)
{
    REQUIRE(a.size() == b.size());

    for (size_t i = 0; i < static_cast<size_t>(a.size()); ++i)
    {
        REQUIRE(std::abs(a(i) - b(i)) < precision);
    }
}

TEST_CASE("Constructors")
{
    SECTION("Name")
    {
        matioCpp::Vector<double> var("test");

        REQUIRE(var.variableType() == matioCpp::VariableType::Vector);
        REQUIRE(var.valueType() == matioCpp::ValueType::DOUBLE);
    }

    SECTION("Name in case of string")
    {
        matioCpp::String var("test");

        REQUIRE(var.variableType() == matioCpp::VariableType::Vector);
        REQUIRE(var.valueType() == matioCpp::ValueType::UTF8);
        REQUIRE(var() == "test");
    }

    SECTION("Name and Dimensions")
    {
        matioCpp::Vector<int64_t> var("test", 7);

        REQUIRE(var.variableType() == matioCpp::VariableType::Vector);
        REQUIRE(var.valueType() == matioCpp::ValueType::INT64);
    }

    SECTION("Name and Span")
    {
        std::vector<int64_t> vec(7);
        matioCpp::Vector<int64_t> var("test", matioCpp::make_span(vec));

        REQUIRE(var.variableType() == matioCpp::VariableType::Vector);
        REQUIRE(var.valueType() == matioCpp::ValueType::INT64);
    }

    SECTION("Name and String")
    {
        matioCpp::Vector<char> var("test", "test_string");

        REQUIRE(var.variableType() == matioCpp::VariableType::Vector);
        REQUIRE(var.valueType() == matioCpp::ValueType::UTF8);
        REQUIRE(var() == "test_string");
    }

    SECTION("Copy constructor")
    {
        std::vector<char> vec(7);
        matioCpp::Vector<char> a("test", matioCpp::make_span(vec));

        matioCpp::Vector<char> b(a);
        REQUIRE(b.variableType() == matioCpp::VariableType::Vector);
        REQUIRE(b.valueType() == matioCpp::ValueType::UTF8);
    }

    SECTION("Move constructor")
    {
        std::vector<char> vec(7);
        matioCpp::Vector<char> a("test", matioCpp::make_span(vec));

        matioCpp::Vector<char> b(std::move(a));
        REQUIRE(b.variableType() == matioCpp::VariableType::Vector);
        REQUIRE(b.valueType() == matioCpp::ValueType::UTF8);
    }

    SECTION("Shared ownership")
    {
        std::vector<double> vec(7);
        std::vector<size_t> dimensions = {vec.size(), 1};
        matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
        REQUIRE(matioVar);

        matioCpp::SharedMatvar sharedMatvar(matioVar);
        matioCpp::Vector<double> sharedVar(sharedMatvar);
        REQUIRE(sharedVar.isValid());
        REQUIRE(sharedVar.toMatio() == matioVar);

        checkVariable(sharedVar, "test", matioCpp::VariableType::Vector,
                      matioCpp::ValueType::DOUBLE, false, dimensions);

        matioCpp::Vector<double> weakVar((matioCpp::WeakMatvar(sharedMatvar)));
        REQUIRE(weakVar.isValid());
        REQUIRE(weakVar.toMatio() == matioVar);

        checkVariable(weakVar, "test", matioCpp::VariableType::Vector,
                      matioCpp::ValueType::DOUBLE, false, dimensions);
    }

}

TEST_CASE("Assignments")
{
    std::vector<int> in = {2,4,6,8};
    matioCpp::Vector<int> out("test");
    out = in; //"in" is automatically converted to span!

    std::vector<double> vec(7);
    std::vector<size_t> dimensions = {vec.size(), 1};
    matvar_t* matioVar = Mat_VarCreate("test", matio_classes::MAT_C_DOUBLE, matio_types::MAT_T_DOUBLE, dimensions.size(), dimensions.data(), vec.data(), 0);
    REQUIRE(matioVar);

    SECTION("From matio")
    {

        matioCpp::Vector<double> var;

        REQUIRE(var.fromMatio(matioVar));

        checkVariable(var, "test", matioCpp::VariableType::Vector, matioCpp::ValueType::DOUBLE, false, dimensions);
    }

    SECTION("From existing vector")
    {
        REQUIRE(out.size() == in.size());
        matioCpp::Span<int> correspondingSpan = out.toSpan();
        REQUIRE(in.size() == static_cast<size_t>(correspondingSpan.size()));

        for (size_t i = 0; i < in.size(); ++i)
        {
            REQUIRE(in[i] == correspondingSpan(i));
            REQUIRE(in[i] == out(i));
            REQUIRE(in[i] == out[i]);
        }
    }

    SECTION("Copy assignement")
    {
        matioCpp::Vector<int> another;

        another = out;

        checkSameVector(another, out);
    }

    SECTION("Move assignement")
    {
        matioCpp::Vector<int> another, yetAnother;

        yetAnother = out;
        another = std::move(yetAnother);

        checkSameVector(another, out);
    }

    SECTION("From other variable (copy)")
    {
        matioCpp::Variable var(matioVar);
        matioCpp::Vector<double> vector;
        REQUIRE(vector.fromOther(var));
        checkSameVariable(var, vector);
    }

    SECTION("From other variable (move)")
    {
        matioCpp::Variable var(matioVar), var2;
        REQUIRE(var2.fromOther(var));
        matioCpp::Vector<double> vector;
        REQUIRE(vector.fromOther(std::move(var2)));
        checkSameVariable(var, vector);
    }

    SECTION("String")
    {
        matioCpp::Vector<char> string;
        string = "test";
        REQUIRE(string() == "test");
    }

    Mat_VarFree(matioVar);
}

TEST_CASE("Span")
{
    std::vector<int> in = {2,4,6,8};
    matioCpp::Vector<int> out("test");
    out = in;

    checkSameVector(matioCpp::make_span(in), out.toSpan());

    in[0] = 3;
    out.toSpan()[0] = 3;

    checkSameVector(matioCpp::make_span(in), out.toSpan());
}

TEST_CASE("Modifications")
{
    std::vector<int> in = {2,4,6,8};
    matioCpp::Vector<int> out("test");
    out = in;

    REQUIRE(out.setName("test2"));
    REQUIRE(out.name() == "test2");
    checkSameVector(matioCpp::make_span(in), out.toSpan());

    in.push_back(10);
    out.resize(5);
    out(4) = 10;
    checkSameVector(matioCpp::make_span(in), out.toSpan());

    in.pop_back();
    in.pop_back();
    out.resize(3);
    checkSameVector(matioCpp::make_span(in), out.toSpan());
}

TEST_CASE("Data")
{
    std::vector<int> in = {2,4,6,8};
    matioCpp::Vector<int> out("test");
    out = in;

    out.data()[2] = 7;
    in[2] = 7;

    checkSameVector(matioCpp::make_span(in), out.toSpan());
}

TEST_CASE("Iterators")
{
    SECTION("iterator_default_init")
    {
        matioCpp::Vector<int>::iterator it1;
        matioCpp::Vector<int>::iterator it2;
        bool ok = it1 == it2;
        REQUIRE(ok);
    }

    SECTION("const_iterator_default_init")
    {
        matioCpp::Vector<int>::const_iterator it1;
        matioCpp::Vector<int>::const_iterator it2;
        bool ok = it1 == it2;
        REQUIRE(ok);
    }

    SECTION("iterator_conversions")
    {
        matioCpp::Vector<int>::iterator badIt;
        matioCpp::Vector<int>::const_iterator badConstIt;
        bool ok = badIt == badConstIt;
        REQUIRE(ok);

        int a[] = {1, 2, 3, 4};
        matioCpp::Vector<int> s("test");
        s = a;

        auto it = s.begin();
        auto cit = s.cbegin();

        ok = it == cit;
        REQUIRE(ok);
        ok = cit == it;
        REQUIRE(ok);

        matioCpp::Vector<int>::const_iterator cit2 = it;
        ok = cit2 == cit;
        REQUIRE(ok);

        matioCpp::Vector<int>::const_iterator cit3 = it + 4;
        ok = cit3 == s.cend();
        REQUIRE(ok);
    }

    SECTION("iterator_comparisons")
    {
        bool ok;
        int a[] = {1, 2, 3, 4};
        {
            matioCpp::Vector<int> s("test");
            s = a;
            matioCpp::Vector<int>::iterator it = s.begin();
            auto it2 = it + 1;
            matioCpp::Vector<int>::const_iterator cit = s.cbegin();

            ok = it == cit;
            REQUIRE(ok);
            ok = cit == it;
            REQUIRE(ok);
            ok = it == it;
            REQUIRE(ok);
            ok = cit == cit;
            REQUIRE(ok);
            ok = cit == s.begin();
            REQUIRE(ok);
            ok = s.begin() == cit;
            REQUIRE(ok);
            ok = s.cbegin() == cit;
            REQUIRE(ok);
            ok = it == s.begin();
            REQUIRE(ok);
            ok = s.begin() == it;
            REQUIRE(ok);

            ok = it != it2;
            REQUIRE(ok);
            ok = it2 != it;
            REQUIRE(ok);
            ok = it != s.end();
            REQUIRE(ok);
            ok = it2 != s.end();
            REQUIRE(ok);
            ok = s.end() != it;
            REQUIRE(ok);
            ok = it2 != cit;
            REQUIRE(ok);
            ok = cit != it2;
            REQUIRE(ok);

            ok = (it < it2);
            REQUIRE(ok);
            ok = (it <= it2);
            REQUIRE(ok);
            ok = (it2 <= s.end());
            REQUIRE(ok);
            ok = (it < s.end());
            REQUIRE(ok);
            ok = (it <= cit);
            REQUIRE(ok);
            ok = (cit <= it);
            REQUIRE(ok);
            ok = (cit < it2);
            REQUIRE(ok);
            ok = (cit <= it2);
            REQUIRE(ok);
            ok = (cit < s.end());
            REQUIRE(ok);
            ok = (cit <= s.end());
            REQUIRE(ok);

            ok = (it2 > it);
            REQUIRE(ok);
            ok = (it2 >= it);
            REQUIRE(ok);
            ok = (s.end() > it2);
            REQUIRE(ok);
            ok = (s.end() >= it2);
            REQUIRE(ok);
            ok = (it2 > cit);
            REQUIRE(ok);
            ok = (it2 >= cit);
            REQUIRE(ok);
        }
    }

    SECTION("begin_end")
    {
        bool ok;
        {
            int a[] = {1, 2, 3, 4};
            matioCpp::Vector<int> s("test");
            s = a;

            matioCpp::Vector<int>::iterator it = s.begin();
            matioCpp::Vector<int>::iterator it2 = std::begin(s);
            ok = (it == it2);
            REQUIRE(ok);

            it = s.end();
            it2 = std::end(s);
            ok = (it == it2);
            REQUIRE(ok);
        }

        {
            int a[] = {1, 2, 3, 4};
            matioCpp::Vector<int> s("test");
            s = a;

            auto it = s.begin();
            auto first = it;
            ok = (it == first);
            REQUIRE(ok);
            ok = (*it == 1);
            REQUIRE(ok);

            auto beyond = s.end();
            ok = (it != beyond);
            REQUIRE(ok);

            ok = (beyond - first == 4);
            REQUIRE(ok);
            ok = (first - first == 0);
            REQUIRE(ok);
            ok = (beyond - beyond == 0);
            REQUIRE(ok);

            ++it;
            ok = (it - first == 1);
            REQUIRE(ok);
            ok = (*it == 2);
            REQUIRE(ok);
            *it = 22;
            ok = (*it == 22);
            REQUIRE(ok);
            ok = (beyond - it == 3);
            REQUIRE(ok);

            it = first;
            ok = (it == first);
            REQUIRE(ok);
            while (it != s.end()) {
                *it = 5;
                ++it;
            }

            ok = (it == beyond);
            REQUIRE(ok);
            ok = (it - beyond == 0);
            REQUIRE(ok);

            for (const auto& n : s) {
                ok = (n == 5);
                REQUIRE(ok);
            }
        }
    }

    SECTION("cbegin_cend")
    {
        bool ok;
        {
            int a[] = {1, 2, 3, 4};
            matioCpp::Vector<int> s("test");
            s = a;

            matioCpp::Vector<int>::const_iterator cit = s.cbegin();
            matioCpp::Vector<int>::const_iterator cit2 = std::cbegin(s);
            ok = (cit == cit2);
            REQUIRE(ok);

            cit = s.cend();
            cit2 = std::cend(s);
            ok = (cit == cit2);
            REQUIRE(ok);
        }

        {
            int a[] = {1, 2, 3, 4};
            matioCpp::Vector<int> s("test");
            s = a;

            auto it = s.cbegin();
            auto first = it;
            ok = (it == first);
            REQUIRE(ok);
            ok = (*it == 1);
            REQUIRE(ok);

            auto beyond = s.cend();
            ok = (it != beyond);
            REQUIRE(ok);

            ok = (beyond - first == 4);
            REQUIRE(ok);
            ok = (first - first == 0);
            REQUIRE(ok);
            ok = (beyond - beyond == 0);
            REQUIRE(ok);

            ++it;
            ok = (it - first == 1);
            REQUIRE(ok);
            ok = (*it == 2);
            REQUIRE(ok);
            ok = (beyond - it == 3);
            REQUIRE(ok);

            int last = 0;
            it = first;
            ok = (it == first);
            REQUIRE(ok);
            while (it != s.cend()) {
                ok = (*it == last + 1);
                REQUIRE(ok);

                last = *it;
                ++it;
            }

            ok = (it == beyond);
            REQUIRE(ok);
            ok = (it - beyond == 0);
            REQUIRE(ok);
        }
    }

    SECTION("rbegin_rend")
    {
        bool ok;
        {
            int a[] = {1, 2, 3, 4};
            matioCpp::Vector<int> s("test");
            s = a;

            auto it = s.rbegin();
            auto first = it;
            ok = (it == first);
            REQUIRE(ok);
            ok = (*it == 4);
            REQUIRE(ok);

            auto beyond = s.rend();
            ok = (it != beyond);
            REQUIRE(ok);

            ok = (beyond - first == 4);
            REQUIRE(ok);
            ok = (first - first == 0);
            REQUIRE(ok);
            ok = (beyond - beyond == 0);
            REQUIRE(ok);

            ++it;
            ok = (it - first == 1);
            REQUIRE(ok);
            ok = (*it == 3);
            REQUIRE(ok);
            *it = 22;
            ok = (*it == 22);
            REQUIRE(ok);
            ok = (beyond - it == 3);
            REQUIRE(ok);

            it = first;
            ok = (it == first);
            REQUIRE(ok);
            while (it != s.rend()) {
                *it = 5;
                ++it;
            }

            ok = (it == beyond);
            REQUIRE(ok);
            ok = (it - beyond == 0);
            REQUIRE(ok);

            for (const auto& n : s) {
                ok = (n == 5);
                REQUIRE(ok);
            }
        }
    }

    SECTION("crbegin_crend")
    {
        bool ok;
        {
            int a[] = {1, 2, 3, 4};
            matioCpp::Vector<int> s("test");
            s = a;

            auto it = s.crbegin();
            auto first = it;
            ok = (it == first);
            REQUIRE(ok);
            ok = (*it == 4);
            REQUIRE(ok);

            auto beyond = s.crend();
            ok = (it != beyond);
            REQUIRE(ok);

            ok = (beyond - first == 4);
            REQUIRE(ok);
            ok = (first - first == 0);
            REQUIRE(ok);
            ok = (beyond - beyond == 0);
            REQUIRE(ok);

            ++it;
            ok = (it - first == 1);
            REQUIRE(ok);
            ok = (*it == 3);
            REQUIRE(ok);
            ok = (beyond - it == 3);
            REQUIRE(ok);

            it = first;
            ok = (it == first);
            REQUIRE(ok);
            int last = 5;
            while (it != s.crend()) {
                ok = (*it == last - 1);
                REQUIRE(ok);
                last = *it;

                ++it;
            }

            ok = (it == beyond);
            REQUIRE(ok);
            ok = (it - beyond == 0);
            REQUIRE(ok);
        }
    }
}

