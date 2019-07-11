#ifndef BURST_TEST_UTILITY_CALLER_DUMMIES_HPP
#define BURST_TEST_UTILITY_CALLER_DUMMIES_HPP

#include <cstddef>
#include <utility>

namespace utility
{
    struct take_first_t
    {
        template <typename X, typename ... Xs>
        auto operator () (X x, Xs &&...) const
        {
            return x;
        }
    };

    template <typename NaryFunction>
    struct const_lvalue_call_counter_t
    {
        explicit const_lvalue_call_counter_t (std::size_t & calls, NaryFunction f):
            calls(calls),
            f(std::move(f))
        {
        }

        template <typename ... Ts>
        auto operator () (Ts... ts) const &
        {
            ++calls;
            return f(ts...);
        }

        template <typename ... Ts>
        auto operator () (Ts...) & = delete;

        template <typename ... Ts>
        auto operator () (Ts...) && = delete;

        std::size_t & calls;
        NaryFunction f;
    };

    template <typename NaryFunction = take_first_t>
    auto const_lvalue_call_counter (std::size_t & calls, NaryFunction f = take_first_t{})
    {
        return const_lvalue_call_counter_t<NaryFunction>{calls, std::move(f)};
    }

    template <typename NaryFunction>
    struct lvalue_call_counter_t
    {
        explicit lvalue_call_counter_t (std::size_t & calls, NaryFunction f):
            calls(calls),
            f(std::move(f))
        {
        }

        template <typename ... Ts>
        auto operator () (Ts... ts) const & = delete;

        template <typename ... Ts>
        auto operator () (Ts... ts) &
        {
            ++calls;
            return f(ts...);
        }

        template <typename ... Ts>
        auto operator () (Ts... ts) && = delete;

        std::size_t & calls;
        NaryFunction f;
    };

    template <typename NaryFunction = take_first_t>
    auto lvalue_call_counter (std::size_t & calls, NaryFunction f = take_first_t{})
    {
        return lvalue_call_counter_t<NaryFunction>{calls, std::move(f)};
    }

    template <typename NaryFunction>
    struct rvalue_call_counter_t
    {
        explicit rvalue_call_counter_t (std::size_t & calls, NaryFunction f):
            calls(calls),
            f(std::move(f))
        {
        }

        template <typename ... Ts>
        auto operator () (Ts...) const & = delete;

        template <typename ... Ts>
        auto operator () (Ts...) & = delete;

        template <typename ... Ts>
        auto operator () (Ts... ts) &&
        {
            ++calls;
            return f(ts...);
        }

        std::size_t & calls;
        NaryFunction f;
    };

    template <typename NaryFunction = take_first_t>
    auto rvalue_call_counter (std::size_t & calls, NaryFunction f = take_first_t{})
    {
        return rvalue_call_counter_t<NaryFunction>{calls, std::move(f)};
    }
}

#endif // BURST_TEST_UTILITY_CALLER_DUMMIES_HPP
