#pragma once
#include "ranges_util.h"

namespace ranges
{
#if USE_RANGEV3
    // Flattens a range of ranges by iterating the inner
    // ranges in round-robin fashion.
    template<class Rngs>
    class interleave_view : public view_facade<interleave_view<Rngs>> {
        friend range_access;
        std::vector<range_value_type_t<Rngs>> rngs_;
        struct cursor;
        cursor begin_cursor() {
            return {0, &rngs_, view::transform(rngs_, begin)};
        }
    public:
        interleave_view() = default;
        explicit interleave_view(Rngs rngs)
          : rngs_(std::move(rngs))
        {}
    };

    template<class Rngs>
    struct interleave_view<Rngs>::cursor  {
        std::size_t n_;
        std::vector<range_value_type_t<Rngs>> *rngs_;
        std::vector<iterator_t<range_value_type_t<Rngs>>> its_;
        decltype(auto) read() const {
            return *its_[n_];
        }
        void next() {
            if(0 == ((++n_) %= its_.size()))
                for_each(its_, [](auto& it){ ++it; });
        }
        bool equal(default_sentinel) const {
            return n_ == 0 && its_.end() != mismatch(its_, *rngs_,
                std::not_equal_to<>(), ident(), end).in1();
        }
        CONCEPT_REQUIRES(ForwardRange<range_value_type_t<Rngs>>())
        bool equal(cursor const& that) const {
            return n_ == that.n_ && its_ == that.its_;
        }
    };

    // In:  Range<Range<T>>
    // Out: Range<T>, flattened by walking the ranges
    //                round-robin fashion.
    inline auto interleave() {
        using namespace ranges;
        return make_pipeable([](auto&& rngs) {
            using Rngs = decltype(rngs);
            return interleave_view<view::all_t<Rngs>>(
                view::all(std::forward<Rngs>(rngs)));
        });
    }
    
#elif (0)
    template <forward_range V>
        requires view<V> && sized_range<V> && input_range<range_reference_t<V>> && sized_range<range_reference_t<V>>
    struct interleave_view : view_interface<interleave_view<V>>
    {
        struct iterator
        {
            using base_t = const V;
            using parent_t = const interleave_view;

            using outer_it = iterator_t<base_t>;
            using inner_it = iterator_t<range_reference_t<base_t>>;

            using iterator_category = std::forward_iterator_tag;
            using value_type = std::invoke_result_t<(decltype(&iterator::current)>;
            using difference_type = std::ptrdiff_t;
            using pointer = void;
            using reference = void;

            constexpr iterator& operator++() { next(); return *this; }

            constexpr iterator() = default;
            constexpr iterator(parent_t* parent)
                : parent_(parent)
            {
                if (parent_ != nullptr)
                {
                    auto sizes = transform_view(*base(), [](auto&& r){ return size(r); });
                    max_idx_ = min(sizes);
                }
            }

            constexpr value_type operator*() const
            {
                return current();
            }

            constexpr bool operator==(const iterator& rhs) const
            {
                if (parent_ != rhs.parent_)
                    return false;

                if (parent_ == nullptr)
                    return true;

                return idx_ == rhs.idx_;
            }

        private:
            constexpr base_t* base() const
            {
                if (parent_)
                    return &parent_->base_;
                return {};
            }

            constexpr void next(range_difference_t<base_t> i)
            {
                idx_ = std::max(idx_ + i, max_idx_);
            }

            constexpr auto current() const
            {
                return transform_view(*base(), 
                    [idx = idx_](auto&& inner) 
                    {  
                        auto it = std::next(inner.begin(), idx);
                        return *it;
                    });
            }

            range_difference_t<base_t> idx_{};
            range_difference_t<base_t> max_idx_{};
            parent_t* parent_{};
        };

        interleave_view() = default;
        constexpr interleave_view(V base) : base_(base) {}

        constexpr V base() const { return base_; }

        constexpr auto begin() const { return iterator(this); }
        constexpr auto end() const { return iterator(nullptr); }
        constexpr auto size() const { return size(base_); }

    private:
        V base_;
    };

    template <typename R>
    interleave_view(R&&) -> interleave_view<all_t<R>>;

    namespace detail
    {
        struct interleave_fn
        {
            template <typename R>
            constexpr auto operator()(R&& r) const
            {
                return interleave_view{std::forward<R>(r)};
            }

            constexpr auto operator()() const
            {
                return piped{[](auto&& r) { return interleave_view{std::forward<decltype(r)>(r)}; }};
            }
        };
    }

    inline namespace views
    {
        constexpr static detail::interleave_fn interleave{};
    }
#endif
}
