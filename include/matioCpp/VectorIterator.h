#ifndef MATIOCPP_VECTORITERATOR_H
#define MATIOCPP_VECTORITERATOR_H

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

//Most of this code has been derived from https://github.com/Microsoft/GSL/blob/master/include/gsl/span
//and https://github.com/robotology/idyntree/blob/master/src/core/include/iDynTree/Core/Span.h

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

namespace matioCpp
{

template <class Vector, bool IsConst>
class vector_iterator
{
    using element_type_ = typename Vector::element_type;
    using input_vector_type = std::conditional_t<IsConst, const Vector, Vector>;

public:

    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::remove_cv_t<element_type_>;
    using difference_type = typename Vector::index_type;

    using reference = std::conditional_t<IsConst, const element_type_, element_type_>&;
    using pointer = std::add_pointer_t<reference>;

    vector_iterator() = default;

    MATIOCPP_CONSTEXPR vector_iterator(input_vector_type* vector, typename Vector::index_type idx) noexcept
        : vector_(vector), index_(idx)
    {}

    friend vector_iterator<Vector, true>;
    template<bool B, std::enable_if_t<!B && IsConst>* = nullptr>
    MATIOCPP_CONSTEXPR vector_iterator(const vector_iterator<Vector, B>& other) noexcept
        : vector_iterator(other.vector_, other.index_)
    {
    }

    MATIOCPP_CONSTEXPR  reference operator*() const
    {
        assert(index_ != vector_->size());
        return *(vector_->data() + index_);
    }

    MATIOCPP_CONSTEXPR pointer operator->() const
    {
        assert(index_ != vector_->size());
        return vector_->data() + index_;
    }

    MATIOCPP_CONSTEXPR vector_iterator& operator++()
    {
        assert(0 <= index_ && index_ != vector_->size());
        ++index_;
        return *this;
    }

    MATIOCPP_CONSTEXPR vector_iterator operator++(int)
    {
        auto ret = *this;
        ++(*this);
        return ret;
    }

    MATIOCPP_CONSTEXPR vector_iterator& operator--()
    {
        assert(index_ != 0 && index_ <= vector_->size());
        --index_;
        return *this;
    }

    MATIOCPP_CONSTEXPR vector_iterator operator--(int)
    {
        auto ret = *this;
        --(*this);
        return ret;
    }

    MATIOCPP_CONSTEXPR vector_iterator operator+(difference_type n) const
    {
        auto ret = *this;
        return ret += n;
    }

    MATIOCPP_CONSTEXPR vector_iterator& operator+=(difference_type n)
    {
        assert((index_ + n) >= 0 && (index_ + n) <= vector_->size());
        index_ += n;
        return *this;
    }

    MATIOCPP_CONSTEXPR vector_iterator operator-(difference_type n) const
    {
        auto ret = *this;
        return ret -= n;
    }

    MATIOCPP_CONSTEXPR vector_iterator& operator-=(difference_type n) { return *this += -n; }

    MATIOCPP_CONSTEXPR difference_type operator-(vector_iterator rhs) const
    {
        assert(vector_ == rhs.vector_);
        return index_ - rhs.index_;
    }

    MATIOCPP_CONSTEXPR reference operator[](difference_type n) const
    {
        return *(*this + n);
    }

    MATIOCPP_CONSTEXPR friend bool operator==(vector_iterator lhs,
                                              vector_iterator rhs) noexcept
    {
        return lhs.vector_ == rhs.vector_ && lhs.index_ == rhs.index_;
    }

    MATIOCPP_CONSTEXPR friend bool operator!=(vector_iterator lhs,
                                              vector_iterator rhs) noexcept
    {
        return !(lhs == rhs);
    }

    MATIOCPP_CONSTEXPR friend bool operator<(vector_iterator lhs,
                                             vector_iterator rhs) noexcept
    {
        return lhs.index_ < rhs.index_;
    }

    MATIOCPP_CONSTEXPR friend bool operator<=(vector_iterator lhs,
                                              vector_iterator rhs) noexcept
    {
        return !(rhs < lhs);
    }

    MATIOCPP_CONSTEXPR friend bool operator>(vector_iterator lhs,
                                             vector_iterator rhs) noexcept
    {
        return rhs < lhs;
    }

    MATIOCPP_CONSTEXPR friend bool operator>=(vector_iterator lhs,
                                              vector_iterator rhs) noexcept
    {
        return !(rhs > lhs);
    }

protected:
    input_vector_type* vector_ = nullptr;
    std::ptrdiff_t index_ = 0;
};

template <class Span, bool IsConst>
MATIOCPP_CONSTEXPR vector_iterator<Span, IsConst>
operator+(typename vector_iterator<Span, IsConst>::difference_type n,
          vector_iterator<Span, IsConst> rhs)
{
    return rhs + n;
}

template <class Span, bool IsConst>
MATIOCPP_CONSTEXPR vector_iterator<Span, IsConst>
operator-(typename vector_iterator<Span, IsConst>::difference_type n,
          vector_iterator<Span, IsConst> rhs)
{
    return rhs - n;
}
}

#endif // MATIOCPP_VECTORITERATOR_H
