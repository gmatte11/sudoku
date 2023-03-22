#pragma once
#include "ranges_util.h"

namespace ranges
{
    template<input_range V, forward_range Pattern>
        requires view<V> && input_range<range_reference_t<V>> && view<Pattern>
    struct join_with_view : public view_interface<join_with_view<V, Pattern>>
    {
        using sentinel = std::default_sentinel_t;
        using value_type = range_value_t<range_reference_t<V>>;

        template <bool Const>
        struct iterator
        {
            using base_t = std::conditional_t<!Const, V, const V>;
            using parent_t = std::conditional_t<!Const, join_with_view, const join_with_view>;

            using iterator_category = typename iterator_t<base_t>::iterator_category;

            using value_type = range_value_t<range_reference_t<base_t>>;
            using difference_type = std::ptrdiff_t;
            using pointer = void;
            using reference = range_reference_t<range_reference_t<base_t>>;

            constexpr iterator() = default;

            constexpr iterator(parent_t* parent, iterator_t<base_t> it) 
                : parent_(parent)
                , outer_it_(it)
            {
                if (outer_it_ != ranges::end(parent->base_))
                    inner_it_ = ranges::begin(*outer_it_);
            }

            constexpr iterator& operator++() { next(); return *this; }
            constexpr iterator operator++(int) const { return iterator(); } // TODO
            //constexpr iterator& operator--() { prev(); return *this; }
            //constexpr iterator operator--(int) const { return iterator(); } // TODO

            constexpr value_type operator*() const
            {
                return current();
            }

            constexpr bool operator==(const iterator& rhs) const
            {
                if (parent_ != rhs.parent_)
                    return false;

                if (parent_ == nullptr)
                    return rhs.parent_ == nullptr || rhs.outer_it_ == ranges::end(rhs.parent_->base_);

                if (outer_it_ == ranges::end(parent_->base_))
                    return rhs.parent_ == nullptr || rhs.outer_it_ == ranges::end(rhs.parent_->base_);

                return outer_it_ == rhs.outer_it_
                    && inner_it_ == rhs.inner_it_
                    && pattern_it_ == rhs.pattern_it_;
            }

            constexpr bool operator==(sentinel) const
                requires (!common_range<V>)
            {
                return parent_ == nullptr || outer_it_ == ranges::end(parent_->base_);
            }

        private:
            constexpr void next()
            {
                if (parent_ != nullptr && outer_it_ != ranges::end(parent_->base_))
                {
                    if (inner_it_ != ranges::end(*outer_it_))
                    {
                        ++inner_it_;
                        if (inner_it_ == ranges::end(*outer_it_))
                        {
                            if (ranges::next(outer_it_) == ranges::end(parent_->base_))
                            {
                                ++outer_it_;
                                return;
                            }
                            pattern_it_ = ranges::begin(parent_->pattern_);
                        }
                    }
                    else 
                    {
                        ++pattern_it_;
                        if (pattern_it_ == ranges::end(parent_->pattern_))
                        {
                            ++outer_it_;
                            if (outer_it_ == ranges::end(parent_->base_))
                                return;
                            inner_it_ = ranges::begin(*outer_it_);
                        }
                    }
                }
            }

            constexpr value_type current() const
            {
                if (parent_ != nullptr && outer_it_ != ranges::end(parent_->base_))
                {
                    if (inner_it_ != ranges::end(*outer_it_))
                        return *inner_it_;
                    else 
                        return *pattern_it_;
                }

                return {};
            }

            iterator_t<base_t> outer_it_{};
            iterator_t<range_value_t<base_t>> inner_it_{};
            iterator_t<const Pattern> pattern_it_{}; 
            const parent_t* parent_{};
        };

        constexpr join_with_view() = default;

        constexpr join_with_view(V v, Pattern p)
            : base_(std::move(v))
            , pattern_(std::move(p))
        {
        }

        template <input_range R>
            requires std::constructible_from<V, views::all_t<R>> &&
                std::constructible_from<Pattern, single_view<value_type>>
        constexpr join_with_view(R&& r, value_type e)
            : base_(views::all(std::forward<R>(r)))
            , pattern_(single_view<value_type>(std::move(e)))
        {
        }

        constexpr auto begin()
        {
            return iterator<false>(this, ranges::begin(base_));
        }

        constexpr auto begin() const
        {
            return iterator<true>(this, ranges::begin(base_));
        }

        constexpr auto end()
        {
            if constexpr (common_range<V>)
            {
                return iterator<false>(this, ranges::end(base_));
            }
            else
            {
                return sentinel();
            }
        }

        constexpr auto end() const
        {
            if constexpr (common_range<V>)
            {
                return iterator<true>(this, ranges::end(base_));
            }
            else
            {
                return sentinel();
            }
        }

    private:
        V base_{};
        Pattern pattern_{};
    };

    template <typename R, typename P>
    join_with_view(R&&, P&&) -> join_with_view<views::all_t<R>, views::all_t<P>>;

    template <typename R>
    join_with_view(R&&, range_value_t<range_reference_t<R>>) -> join_with_view<views::all_t<R>, single_view<range_value_t<range_reference_t<R>>>>;

    namespace detail
    {
        struct join_with_view_fn
        {
            template <typename P>
            constexpr auto operator()(P&& p) const
            {
                return piped{[p = std::forward<P>(p)](auto&& r) { return join_with_view(std::forward<decltype(r)>(r), std::move(p)); }};
            }

            template <typename R, typename P>
            constexpr auto operator()(R&& r, P&& p) const
            {
                return join_with_view(std::forward<R>(r), std::forward<P>(p));
            }
        };
    }

    namespace views
    {
        inline constexpr detail::join_with_view_fn join_with{};
    }
}