///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

//Most of this file has been taken from https://github.com/Microsoft/GSL/blob/master/tests/span_tests.cpp
// and https://github.com/robotology/idyntree/blob/master/src/core/tests/SpanUnitTest.cpp

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

#include <matioCpp/matioCpp.h>

// Catch2
#include <catch2/catch.hpp>

#include <array>       // for array
#include <iostream>    // for ptrdiff_t
#include <iterator>    // for reverse_iterator, operator-, operator==
#include <memory>      // for unique_ptr, shared_ptr, make_unique, allo...
#include <regex>       // for match_results, sub_match, match_results<>...
#include <stddef.h>    // for ptrdiff_t
#include <string>      // for string
#include <type_traits> // for integral_constant<>::value, is_default_co...
#include <vector>      // for vector

using namespace std;
using namespace matioCpp;

namespace
{
struct BaseClass
{
};
struct DerivedClass : BaseClass
{
};
}

TEST_CASE("default_constructor")
{
    {
        Span<int> s;
        REQUIRE((s.size() == 0 && s.data() == nullptr));

        Span<const int> cs;
        REQUIRE((cs.size() == 0 && cs.data() == nullptr));
    }

    {
        Span<int, 0> s;
        REQUIRE((s.size() == 0 && s.data() == nullptr));

        Span<const int, 0> cs;
        REQUIRE((cs.size() == 0 && cs.data() == nullptr));
    }

    {
        Span<int> s{};
        REQUIRE((s.size() == 0 && s.data() == nullptr));

        Span<const int> cs{};
        REQUIRE((cs.size() == 0 && cs.data() == nullptr));
    }
}

TEST_CASE("size_optimization")
{
    {
        Span<int> s;
        REQUIRE(sizeof(s) == sizeof(int*) + sizeof(ptrdiff_t));
    }

    {
        Span<int, 0> s;
        REQUIRE(sizeof(s) == sizeof(int*));
    }
}

TEST_CASE("from_nullptr_size_constructor")
{
    {
        Span<int> s{nullptr, static_cast<Span<int>::index_type>(0)};
        REQUIRE((s.size() == 0 && s.data() == nullptr));

        Span<const int> cs{nullptr, static_cast<Span<int>::index_type>(0)};
        REQUIRE((cs.size() == 0 && cs.data() == nullptr));
    }

    {
        Span<int, 0> s{nullptr, static_cast<Span<int>::index_type>(0)};
        REQUIRE((s.size() == 0 && s.data() == nullptr));

        Span<const int, 0> cs{nullptr, static_cast<Span<int>::index_type>(0)};
        REQUIRE((cs.size() == 0 && cs.data() == nullptr));
    }

    {
        Span<int*> s{nullptr, static_cast<Span<int>::index_type>(0)};
        REQUIRE((s.size() == 0 && s.data() == nullptr));

        Span<const int*> cs{nullptr, static_cast<Span<int>::index_type>(0)};
        REQUIRE((cs.size() == 0 && cs.data() == nullptr));
    }
}

TEST_CASE("from_pointer_length_constructor")
{
    int arr[4] = {1, 2, 3, 4};

    {
        for(int i = 0; i<4 ; ++i)
        {
            {
                Span<int> s = { &arr[0], i };
                REQUIRE(s.size() == i);
                REQUIRE(s.data() == &arr[0]);
                REQUIRE(s.empty() == (i == 0));
                for (int j = 0; j < i; ++j)
                {
                    REQUIRE(arr[j] == s[j]);
                    REQUIRE(arr[j] == s.at(j));
                    REQUIRE(arr[j] == s(j));
                }
            }
            {
                Span<int> s = { &arr[i], 4-i };
                REQUIRE(s.size() == 4-i);
                REQUIRE(s.data() == &arr[i]);
                REQUIRE(s.empty() == (4-i == 0));
                for (int j = 0; j < 4-i; ++j)
                {
                    REQUIRE(arr[j+i] == s[j]);
                    REQUIRE(arr[j+i] == s.at(j));
                    REQUIRE(arr[j+i] == s(j));
                }
            }
        }
    }

    {
        Span<int, 2> s{&arr[0], 2};
        REQUIRE((s.size() == 2 && s.data() == &arr[0]));
        REQUIRE((s[0] == 1 && s[1] == 2));
    }

    {
        int* p = nullptr;
        Span<int> s{p, static_cast<Span<int>::index_type>(0)};
        REQUIRE((s.size() == 0 && s.data() == nullptr));
    }

    {
        auto s = make_span(&arr[0], 2);
        REQUIRE((s.size() == 2 && s.data() == &arr[0]));
        REQUIRE((s[0] == 1 && s[1] == 2));
    }

    {
        int* p = nullptr;
        auto s = make_span(p, static_cast<Span<int>::index_type>(0));
        REQUIRE((s.size() == 0 && s.data() == nullptr));
    }

}

TEST_CASE("from_pointer_pointer_constructor")
{
    int arr[4] = {1, 2, 3, 4};

    {
        Span<int> s{&arr[0], &arr[2]};
        REQUIRE((s.size() == 2 && s.data() == &arr[0]));
        REQUIRE((s[0] == 1 && s[1] == 2));
    }

    {
        Span<int, 2> s{&arr[0], &arr[2]};
        REQUIRE((s.size() == 2 && s.data() == &arr[0]));
        REQUIRE((s[0] == 1 && s[1] == 2));
    }

    {
        Span<int> s{&arr[0], &arr[0]};
        REQUIRE((s.size() == 0 && s.data() == &arr[0]));
    }

    {
        Span<int, 0> s{&arr[0], &arr[0]};
        REQUIRE((s.size() == 0 && s.data() == &arr[0]));
    }

    {
        int* p = nullptr;
        Span<int> s{p, p};
        REQUIRE((s.size() == 0 && s.data() == nullptr));
    }

    {
        int* p = nullptr;
        Span<int, 0> s{p, p};
        REQUIRE((s.size() == 0 && s.data() == nullptr));
    }

    {
        auto s = make_span(&arr[0], &arr[2]);
        REQUIRE((s.size() == 2 && s.data() == &arr[0]));
        REQUIRE((s[0] == 1 && s[1] == 2));
    }

    {
        auto s = make_span(&arr[0], &arr[0]);
        REQUIRE((s.size() == 0 && s.data() == &arr[0]));
    }

    {
        int* p = nullptr;
        auto s = make_span(p, p);
        REQUIRE((s.size() == 0 && s.data() == nullptr));
    }
}

TEST_CASE("from_array_constructor")
{
    int arr[5] = {1, 2, 3, 4, 5};

    {
        Span<int> s{arr};
        REQUIRE((s.size() == 5 && s.data() == &arr[0]));
    }

    {
        Span<int, 5> s{arr};
        REQUIRE((s.size() == 5 && s.data() == &arr[0]));
    }

    int arr2d[2][3] = {1, 2, 3, 4, 5, 6};

    {
        Span<int[3]> s{&(arr2d[0]), 1};
        REQUIRE((s.size() == 1 && s.data() == &arr2d[0]));
    }

    int arr3d[2][3][2] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

    {
        Span<int[3][2]> s{&arr3d[0], 1};
        REQUIRE((s.size() == 1 && s.data() == &arr3d[0]));
    }

    {
        auto s = make_span(arr);
        REQUIRE((s.size() == 5 && s.data() == &arr[0]));
    }

    {
        auto s = make_span(&(arr2d[0]), 1);
        REQUIRE((s.size() == 1 && s.data() == &arr2d[0]));
    }

    {
        auto s = make_span(&arr3d[0], 1);
        REQUIRE((s.size() == 1 && s.data() == &arr3d[0]));
    }
}

TEST_CASE("from_dynamic_array_constructor")
{
    double(*arr)[3][4] = new double[100][3][4];

    {
        Span<double> s(&arr[0][0][0], 10);
        REQUIRE((s.size() == 10 && s.data() == &arr[0][0][0]));
    }

    {
        auto s = make_span(&arr[0][0][0], 10);
        REQUIRE((s.size() == 10 && s.data() == &arr[0][0][0]));
    }

    delete[] arr;
}

TEST_CASE("from_std_array_constructor")
{
    std::array<int, 4> arr = {1, 2, 3, 4};

    {
        Span<int> s{arr};
        REQUIRE((s.size() == static_cast<ptrdiff_t>(arr.size()) && s.data() == arr.data()));

        Span<const int> cs{arr};
        REQUIRE((cs.size() == static_cast<ptrdiff_t>(arr.size()) && cs.data() == arr.data()));
    }

    {
        Span<int, 4> s{arr};
        REQUIRE((s.size() == static_cast<ptrdiff_t>(arr.size()) && s.data() == arr.data()));

        Span<const int, 4> cs{arr};
        REQUIRE((cs.size() == static_cast<ptrdiff_t>(arr.size()) && cs.data() == arr.data()));
    }

    {
        auto get_an_array = []() -> std::array<int, 4> { return {1, 2, 3, 4}; };
        auto take_a_span = [](Span<const int> s) { static_cast<void>(s); };
        // try to take a temporary std::array
        take_a_span(get_an_array());
    }

    {
        auto s = make_span(arr);
        REQUIRE((s.size() == static_cast<ptrdiff_t>(arr.size()) && s.data() == arr.data()));
    }
}

TEST_CASE("from_const_std_array_constructor")
{
    const std::array<int, 4> arr = {1, 2, 3, 4};

    {
        Span<const int> s{arr};
        REQUIRE((s.size() == static_cast<ptrdiff_t>(arr.size()) && s.data() == arr.data()));
    }

    {
        Span<const int, 4> s{arr};
        REQUIRE((s.size() == static_cast<ptrdiff_t>(arr.size()) && s.data() == arr.data()));
    }

    {
        auto get_an_array = []() -> const std::array<int, 4> { return {1, 2, 3, 4}; };
        auto take_a_span = [](Span<const int> s) { static_cast<void>(s); };
        // try to take a temporary std::array
        take_a_span(get_an_array());
    }

    {
        auto s = make_span(arr);
        REQUIRE((s.size() == static_cast<ptrdiff_t>(arr.size()) && s.data() == arr.data()));
    }
}

TEST_CASE("from_std_array_const_constructor")
{
    std::array<const int, 4> arr = {1, 2, 3, 4};

    {
        Span<const int> s{arr};
        REQUIRE((s.size() == static_cast<ptrdiff_t>(arr.size()) && s.data() == arr.data()));
    }

    {
        Span<const int, 4> s{arr};
        REQUIRE((s.size() == static_cast<ptrdiff_t>(arr.size()) && s.data() == arr.data()));
    }

    {
        auto s = make_span(arr);
        REQUIRE((s.size() == static_cast<ptrdiff_t>(arr.size()) && s.data() == arr.data()));
    }
}

TEST_CASE("from_container_constructor")
{
    std::vector<int> v = {1, 2, 3};
    const std::vector<int> cv = v;

    {
        Span<int> s{v};
        REQUIRE((s.size() == static_cast<std::ptrdiff_t>(v.size()) && s.data() == v.data()));

        Span<const int> cs{v};
        REQUIRE((cs.size() == static_cast<std::ptrdiff_t>(v.size()) && cs.data() == v.data()));
    }

    std::string str = "hello";
    const std::string cstr = "hello";

    {
        Span<const char> cs{str};
        REQUIRE((cs.size() == static_cast<std::ptrdiff_t>(str.size()) && cs.data() == str.data()));
    }

    {
        Span<const char> cs{cstr};
        REQUIRE((cs.size() == static_cast<std::ptrdiff_t>(cstr.size()) &&
              cs.data() == cstr.data()));
    }

    {
        auto get_temp_vector = []() -> std::vector<int> { return {}; };
        auto use_span = [](Span<const int> s) { static_cast<void>(s); };
        use_span(get_temp_vector());
    }

    {
        auto get_temp_string = []() -> std::string { return {}; };
        auto use_span = [](Span<const char> s) { static_cast<void>(s); };
        use_span(get_temp_string());
    }

    {
        auto get_temp_string = []() -> const std::string { return {}; };
        auto use_span = [](Span<const char> s) { static_cast<void>(s); };
        use_span(get_temp_string());
    }

    {
        auto s = make_span(v);
        REQUIRE((s.size() == static_cast<std::ptrdiff_t>(v.size()) && s.data() == v.data()));

        auto cs = make_span(cv);
        REQUIRE((cs.size() == static_cast<std::ptrdiff_t>(cv.size()) && cs.data() == cv.data()));
    }
}

TEST_CASE("from_convertible_span_constructor")
{
    Span<DerivedClass> avd;
    Span<const DerivedClass> avcd = avd;
    static_cast<void>(avcd);
}

TEST_CASE("copy_move_and_assignment")
{
    Span<int> s1;
    REQUIRE(s1.empty());

    int arr[] = {3, 4, 5};

    Span<const int> s2 = arr;
    REQUIRE((s2.size() == 3 && s2.data() == &arr[0]));

    s2 = s1;
    REQUIRE(s2.empty());

    auto get_temp_span = [&]() -> Span<int> { return {&arr[1], 2}; };
    auto use_span = [&](Span<const int> s) { REQUIRE((s.size() == 2 && s.data() == &arr[1])); };
    use_span(get_temp_span());

    s1 = get_temp_span();
    REQUIRE((s1.size() == 2 && s1.data() == &arr[1]));
}

TEST_CASE("first")
{
    int arr[5] = {1, 2, 3, 4, 5};

    {
        Span<int, 5> av = arr;
        bool ok;
        ok = av.first<2>().size() == 2;
        REQUIRE(ok);
        ok = av.first(2).size() == 2;
        REQUIRE(ok);
    }

    {
        Span<int, 5> av = arr;
        REQUIRE(av.first<0>().size() == 0);
        REQUIRE(av.first(0).size() == 0);
    }

    {
        Span<int, 5> av = arr;
        REQUIRE(av.first<5>().size() == 5);
        REQUIRE(av.first(5).size() == 5);
    }

    {
        Span<int> av;
        REQUIRE(av.first<0>().size() == 0);
        REQUIRE(av.first(0).size() == 0);
    }
}

TEST_CASE("last")
{
    int arr[5] = {1, 2, 3, 4, 5};

    {
        Span<int, 5> av = arr;
        REQUIRE(av.last<2>().size() == 2);
        REQUIRE(av.last(2).size() == 2);
    }

    {
        Span<int, 5> av = arr;
        REQUIRE(av.last<0>().size() == 0);
        REQUIRE(av.last(0).size() == 0);
    }

    {
        Span<int, 5> av = arr;
        REQUIRE(av.last<5>().size() == 5);
        REQUIRE(av.last(5).size() == 5);
    }

    {
        Span<int> av;
        REQUIRE(av.last<0>().size() == 0);
        REQUIRE(av.last(0).size() == 0);
    }
}

TEST_CASE("subspan")
{
    int arr[5] = {1, 2, 3, 4, 5};

    {
        Span<int, 5> av = arr;
        REQUIRE((av.subspan<2, 2>().size() == 2));
        REQUIRE(decltype(av.subspan<2, 2>())::extent == 2);
        REQUIRE(av.subspan(2, 2).size() == 2);
        REQUIRE(av.subspan(2, 3).size() == 3);
    }

    {
        Span<int, 5> av = arr;
        REQUIRE((av.subspan<0, 0>().size() == 0));
        REQUIRE(decltype(av.subspan<0,0>())::extent == 0);
        REQUIRE(av.subspan(0, 0).size() == 0);
    }

    {
        Span<int, 5> av = arr;
        REQUIRE((av.subspan<0, 5>().size() == 5));
        REQUIRE(decltype(av.subspan<0, 5>())::extent == 5);
        REQUIRE(av.subspan(0, 5).size() == 5);

    }

    {
        Span<int, 5> av = arr;
        REQUIRE((av.subspan<4, 0>().size() == 0));
        REQUIRE(decltype(av.subspan<4, 0>())::extent == 0);
        REQUIRE(av.subspan(4, 0).size() == 0);
        REQUIRE(av.subspan(5, 0).size() == 0);
    }

    {
        Span<int, 5> av = arr;
        REQUIRE((av.subspan<1>().size() == 4));
        REQUIRE(decltype(av.subspan<1>())::extent == 4);
    }

    {
        Span<int> av;
        REQUIRE((av.subspan<0, 0>().size() == 0));
        REQUIRE((decltype(av.subspan<0, 0>())::extent == 0));
        REQUIRE(av.subspan(0, 0).size() == 0);
    }

    {
        Span<int> av;
        REQUIRE(av.subspan(0).size() == 0);
    }

    {
        Span<int> av = arr;
        REQUIRE(av.subspan(0).size() == 5);
        REQUIRE(av.subspan(1).size() == 4);
        REQUIRE(av.subspan(4).size() == 1);
        REQUIRE(av.subspan(5).size() == 0);
        const auto av2 = av.subspan(1);
        for (int i = 0; i < 4; ++i) REQUIRE(av2[i] == i + 2);
    }

    {
        Span<int, 5> av = arr;
        REQUIRE(av.subspan(0).size() == 5);
        REQUIRE(av.subspan(1).size() == 4);
        REQUIRE(av.subspan(4).size() == 1);
        REQUIRE(av.subspan(5).size() == 0);
        const auto av2 = av.subspan(1);
        for (int i = 0; i < 4; ++i) REQUIRE(av2[i] == i + 2);
    }
}

TEST_CASE("at_call")
{
    int arr[4] = {1, 2, 3, 4};

    {
        Span<int> s = arr;
        REQUIRE(s.at(0) == 1);
    }

    {
        int arr2d[2] = {1, 6};
        Span<int, 2> s = arr2d;
        REQUIRE(s.at(0) == 1);
        REQUIRE(s.at(1) == 6);
    }
}

TEST_CASE("operator_function_call")
{
    int arr[4] = {1, 2, 3, 4};

    {
        Span<int> s = arr;
        REQUIRE(s(0) == 1);
    }

    {
        int arr2d[2] = {1, 6};
        Span<int, 2> s = arr2d;
        REQUIRE(s(0) == 1);
        REQUIRE(s(1) == 6);
    }
}

TEST_CASE("iterator_default_init")
{
    Span<int>::iterator it1;
    Span<int>::iterator it2;
    bool ok = it1 == it2;
    REQUIRE(ok);
}

TEST_CASE("const_iterator_default_init")
{
    Span<int>::const_iterator it1;
    Span<int>::const_iterator it2;
    bool ok = it1 == it2;
    REQUIRE(ok);
}

TEST_CASE("iterator_conversions")
{
    Span<int>::iterator badIt;
    Span<int>::const_iterator badConstIt;
    bool ok = badIt == badConstIt;
    REQUIRE(ok);

    int a[] = {1, 2, 3, 4};
    Span<int> s = a;

    auto it = s.begin();
    auto cit = s.cbegin();

    ok = it == cit;
    REQUIRE(ok);
    ok = cit == it;
    REQUIRE(ok);

    Span<int>::const_iterator cit2 = it;
    ok = cit2 == cit;
    REQUIRE(ok);

    Span<int>::const_iterator cit3 = it + 4;
    ok = cit3 == s.cend();
    REQUIRE(ok);
}

TEST_CASE("iterator_comparisons")
{
    bool ok;
    int a[] = {1, 2, 3, 4};
    {
        Span<int> s = a;
        Span<int>::iterator it = s.begin();
        auto it2 = it + 1;
        Span<int>::const_iterator cit = s.cbegin();

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

TEST_CASE("begin_end")
{
    bool ok;
    {
        int a[] = {1, 2, 3, 4};
        Span<int> s = a;

        Span<int>::iterator it = s.begin();
        Span<int>::iterator it2 = std::begin(s);
        ok = (it == it2);
        REQUIRE(ok);

        it = s.end();
        it2 = std::end(s);
        ok = (it == it2);
        REQUIRE(ok);
    }

    {
        int a[] = {1, 2, 3, 4};
        Span<int> s = a;

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

TEST_CASE("cbegin_cend")
{
    bool ok;
    {
        int a[] = {1, 2, 3, 4};
        Span<int> s = a;

        Span<int>::const_iterator cit = s.cbegin();
        Span<int>::const_iterator cit2 = std::cbegin(s);
        ok = (cit == cit2);
        REQUIRE(ok);

        cit = s.cend();
        cit2 = std::cend(s);
        ok = (cit == cit2);
        REQUIRE(ok);
    }

    {
        int a[] = {1, 2, 3, 4};
        Span<int> s = a;

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

TEST_CASE("rbegin_rend")
{
    bool ok;
    {
        int a[] = {1, 2, 3, 4};
        Span<int> s = a;

        auto it = s.rbegin();
        auto first = it;
        ok = (it == first);
        REQUIRE(ok);
        ok = (*it == 4);
        REQUIRE(ok);

        auto beyond = s.rend();
        ok = (it != beyond);
        REQUIRE(ok);
        //CHECK_THROWS_AS(*beyond, fail_fast);

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

TEST_CASE("crbegin_crend")
{
    bool ok;
    {
        int a[] = {1, 2, 3, 4};
        Span<int> s = a;

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

TEST_CASE("comparison_operators")
{
    {
        Span<int> s1;
        Span<int> s2;
        REQUIRE(s1 == s2);
        REQUIRE(!(s1 != s2));
        REQUIRE(!(s1 < s2));
        REQUIRE(s1 <= s2);
        REQUIRE(!(s1 > s2));
        REQUIRE(s1 >= s2);
        REQUIRE(s2 == s1);
        REQUIRE(!(s2 != s1));
        REQUIRE(!(s2 < s1));
        REQUIRE(s2 <= s1);
        REQUIRE(!(s2 > s1));
        REQUIRE(s2 >= s1);
    }

    {
        int arr[] = {2, 1};
        Span<int> s1 = arr;
        Span<int> s2 = arr;

        REQUIRE(s1 == s2);
        REQUIRE(!(s1 != s2));
        REQUIRE(!(s1 < s2));
        REQUIRE(s1 <= s2);
        REQUIRE(!(s1 > s2));
        REQUIRE(s1 >= s2);
        REQUIRE(s2 == s1);
        REQUIRE(!(s2 != s1));
        REQUIRE(!(s2 < s1));
        REQUIRE(s2 <= s1);
        REQUIRE(!(s2 > s1));
        REQUIRE(s2 >= s1);
    }

    {
        int arr[] = {2, 1}; // bigger

        Span<int> s1;
        Span<int> s2 = arr;

        REQUIRE(s1 != s2);
        REQUIRE(s2 != s1);
        REQUIRE(!(s1 == s2));
        REQUIRE(!(s2 == s1));
        REQUIRE(s1 < s2);
        REQUIRE(!(s2 < s1));
        REQUIRE(s1 <= s2);
        REQUIRE(!(s2 <= s1));
        REQUIRE(s2 > s1);
        REQUIRE(!(s1 > s2));
        REQUIRE(s2 >= s1);
        REQUIRE(!(s1 >= s2));
    }

    {
        int arr1[] = {1, 2};
        int arr2[] = {1, 2};
        Span<int> s1 = arr1;
        Span<int> s2 = arr2;

        REQUIRE(s1 == s2);
        REQUIRE(!(s1 != s2));
        REQUIRE(!(s1 < s2));
        REQUIRE(s1 <= s2);
        REQUIRE(!(s1 > s2));
        REQUIRE(s1 >= s2);
        REQUIRE(s2 == s1);
        REQUIRE(!(s2 != s1));
        REQUIRE(!(s2 < s1));
        REQUIRE(s2 <= s1);
        REQUIRE(!(s2 > s1));
        REQUIRE(s2 >= s1);
    }

    {
        int arr[] = {1, 2, 3};

        Span<int> s1 = {&arr[0], 2}; // shorter
        Span<int> s2 = arr;          // longer

        REQUIRE(s1 != s2);
        REQUIRE(s2 != s1);
        REQUIRE(!(s1 == s2));
        REQUIRE(!(s2 == s1));
        REQUIRE(s1 < s2);
        REQUIRE(!(s2 < s1));
        REQUIRE(s1 <= s2);
        REQUIRE(!(s2 <= s1));
        REQUIRE(s2 > s1);
        REQUIRE(!(s1 > s2));
        REQUIRE(s2 >= s1);
        REQUIRE(!(s1 >= s2));
    }

    {
        int arr1[] = {1, 2}; // smaller
        int arr2[] = {2, 1}; // bigger

        Span<int> s1 = arr1;
        Span<int> s2 = arr2;

        REQUIRE(s1 != s2);
        REQUIRE(s2 != s1);
        REQUIRE(!(s1 == s2));
        REQUIRE(!(s2 == s1));
        REQUIRE(s1 < s2);
        REQUIRE(!(s2 < s1));
        REQUIRE(s1 <= s2);
        REQUIRE(!(s2 <= s1));
        REQUIRE(s2 > s1);
        REQUIRE(!(s1 > s2));
        REQUIRE(s2 >= s1);
        REQUIRE(!(s1 >= s2));
    }
}

TEST_CASE("fixed_size_conversions")
{
    int arr[] = {1, 2, 3, 4};

    // converting to an Span from an equal size array is ok
    Span<int, 4> s4 = arr;
    REQUIRE(s4.size() == 4);

    // converting to dynamic_range is always ok
    {
        Span<int> s = s4;
        REQUIRE(s.size() == s4.size());
        static_cast<void>(s);
    }

// initialization or assignment to static Span that REDUCES size is NOT ok
#ifdef CONFIRM_COMPILATION_ERRORS
    {
        Span<int, 2> s = arr;
    }
    {
        Span<int, 2> s2 = s4;
        static_cast<void>(s2);
    }
#endif

    // even when done dynamically
    {
        Span<int> s = arr;
        auto f = [&]() {
            Span<int, 2> s2 = s;
            static_cast<void>(s2);
        };
//        CHECK_THROWS_AS(f(), fail_fast);
    }

    // but doing so explicitly is ok

    // you can convert statically
    {
        const Span<int, 2> s2 = {arr, 2};
        static_cast<void>(s2);
    }
    {
        const Span<int, 1> s1 = s4.first<1>();
        static_cast<void>(s1);
    }

    // ...or dynamically
    {
        // NB: implicit conversion to Span<int,1> from Span<int>
        Span<int, 1> s1 = s4.first(1);
        static_cast<void>(s1);
    }

    // initialization or assignment to static Span that requires size INCREASE is not ok.
    int arr2[2] = {1, 2};

#ifdef CONFIRM_COMPILATION_ERRORS
    {
        Span<int, 4> s3 = arr2;
    }
    {
        Span<int, 2> s2 = arr2;
        Span<int, 4> s4a = s2;
    }
#endif
    {
        auto f = [&]() {
            Span<int, 4> _s4 = {arr2, 2};
            static_cast<void>(_s4);
        };
//        CHECK_THROWS_AS(f(), fail_fast);
    }

    // this should fail - we are trying to assign a small dynamic Span to a fixed_size larger one
    Span<int> av = arr2;
    auto f = [&]() {
        Span<int, 4> _s4 = av;
        static_cast<void>(_s4);
    };
//    CHECK_THROWS_AS(f(), fail_fast);
}

TEST_CASE("interop_with_std_regex")
{
    char lat[] = {'1', '2', '3', '4', '5', '6', 'E', 'F', 'G'};
    Span<char> s = lat;
    const auto f_it = s.begin() + 7;

    std::match_results<Span<char>::iterator> match;

    std::regex_match(s.begin(), s.end(), match, std::regex(".*"));
    REQUIRE(match.ready());
    REQUIRE(!match.empty());
    REQUIRE(match[0].matched);
    REQUIRE(match[0].first == s.begin());
    REQUIRE(match[0].second == s.end());

    std::regex_search(s.begin(), s.end(), match, std::regex("F"));
    REQUIRE(match.ready());
    REQUIRE(!match.empty());
    REQUIRE(match[0].matched);
    REQUIRE(match[0].first == f_it);
    REQUIRE(match[0].second == (f_it + 1));
}

TEST_CASE("default_constructible")
{
    REQUIRE((std::is_default_constructible<Span<int>>::value));
    REQUIRE((std::is_default_constructible<Span<int, 0>>::value));
    REQUIRE((!std::is_default_constructible<Span<int, 42>>::value));
}
