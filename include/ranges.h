#pragma once
#include <nanorange.hpp>

#include <coroutine>

#define USE_RANGEV3 0
#define USE_NANORANGE 1
#define USE_STDRANGE 0

namespace ranges = nano::ranges;
namespace rng = ranges;

namespace sudoku::views 
{
    using namespace nano::views;

    namespace detail
    {
        template <typename T>
        struct range_generator
        {
            using value_type = std::remove_cvref_t<T>;

            struct promise_type;
            using handle_type = std::coroutine_handle<promise_type>;

            struct promise_type
            {
                value_type value_{};
                bool has_value_ = false;
                std::exception_ptr ex_;

                range_generator get_return_object()
                {
                    return range_generator(handle_type::from_promise(*this));
                }

                std::suspend_always initial_suspend() { return {}; }
                std::suspend_always final_suspend() noexcept { return {}; }

                void unhandled_exception() { ex_ = std::current_exception(); }

                template <std::convertible_to<T> From>
                std::suspend_always yield_value(From&& from)
                {
                    value_ = std::forward<From>(from);
                    has_value_ = true;
                    return {};
                }

                void return_void()
                {
                    has_value_ = false;
                };

                value_type get_value()
                {
                    return std::move(value_);
                }

                bool done() const
                {
                    return !has_value_;
                }
            };

            struct iterator
            {
                using value_type = value_type;
                using iterator_category = std::input_iterator_tag;
                using difference_type = std::ptrdiff_t;

                iterator() = default;
                iterator(range_generator& gen) : gen_(&gen) {}

                value_type operator*() const
                {
                    if (gen_)
                        return gen_->h_.promise().get_value();
                    return {};
                }

                iterator& operator++()
                {
                    if (gen_ && gen_->h_)
                        gen_->h_.resume();
                    return *this;
                }

                iterator& operator++(int)
                {
                    return ++(*this);
                }

                bool operator==(rng::default_sentinel_t) const
                {
                    return gen_ ? gen_->h_.promise().done() : true;
                }

            private:
                range_generator* gen_{};
            };

            handle_type h_;

#if USE_NANORANGE
            range_generator() = default; // for nano::ranges::view
#endif
            range_generator(handle_type h) : h_(h) {}
            ~range_generator() { h_.destroy(); }

            explicit operator bool()
            {
                fill();
                return !h_.done();
            }

            T operator()()
            {
                fill();
                full_ = false;
                return std::move(h_.promise().value_);
            }

            iterator begin()
            {
                auto it = iterator(*this);
                return ++it;
            }

            rng::default_sentinel_t end()
            {
                return rng::default_sentinel_t{};
            }

        private:
            bool full_ = false;

            void fill()
            {
                if (!full_)
                {
                    h_();
                    if (h_.promise().ex_)
                        std::rethrow_exception(h_.promise().ex_);
                    full_ = true;
                }
            }
        };
    }

    // join_with not available in nanoranges
    template <typename R, typename T>
    inline detail::range_generator<T> join_with(R&& r, T&& t)
    {
        bool first = true;
        for (auto&& sub : r)
        {
            if (!first)
                co_yield t;
            first = false;

            for (auto &&v : sub)
                co_yield static_cast<T>(v);
        }
    }

    template <typename T>
    inline auto join_with(T&& t)
    {
#if USE_NANORANGE
        return ranges::detail::rao_proxy{[t = std::move(t)](auto&& r){ return join_with(r, t); }};
#endif
    }
}