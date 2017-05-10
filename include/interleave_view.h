#pragma once
#include <range/v3/all.hpp>
/* Taken from Eric Niebler's Calendar example.
   https://github.com/ericniebler/range-v3/blob/master/example/calendar.cpp
*/

namespace sudoku
{
    // Flattens a range of ranges by iterating the inner
    // ranges in round-robin fashion.
    template<class Rngs>
    class interleave_view : public ranges::view_facade<interleave_view<Rngs>> {
        friend ranges::range_access;
        std::vector<ranges::range_value_type_t<Rngs>> rngs_;
        struct cursor;
        cursor begin_cursor() {
            return {0, &rngs_, ranges::view::transform(rngs_, ranges::begin)};
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
        std::vector<ranges::range_value_type_t<Rngs>> *rngs_;
        std::vector<ranges::iterator_t<ranges::range_value_type_t<Rngs>>> its_;
        decltype(auto) read() const {
            return *its_[n_];
        }
        void next() {
            if(0 == ((++n_) %= its_.size()))
                ranges::for_each(its_, [](auto& it){ ++it; });
        }
        bool equal(ranges::default_sentinel) const {
            return n_ == 0 && its_.end() != ranges::mismatch(its_, *rngs_,
                std::not_equal_to<>(), ranges::ident(), ranges::end).in1();
        }
        CONCEPT_REQUIRES(ranges::ForwardRange<ranges::range_value_type_t<Rngs>>())
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
}
