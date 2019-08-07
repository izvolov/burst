#include <burst/algorithm/sum.hpp>
#include <burst/functional/only.hpp>
#include <burst/integer/intlog2.hpp>
#include <burst/tuple/make_tuple.hpp>
#include <utility/caller_dummies.hpp>
#include <utility/io/tuple.hpp>

#include <doctest/doctest.h>

#include <functional>
#include <string>
#include <tuple>

namespace // anonymous
{
    struct dummy
    {
        dummy ()
        {
            ++instances_count;
        }

        dummy (const dummy &)
        {
            ++instances_count;
        }

        dummy (dummy &&)
        {
            ++instances_count;
        }

        ~dummy ()
        {
            --instances_count;
        }

        static int instances_count;
    };

    int dummy::instances_count = 0;
} // namespace anonymous

TEST_SUITE("only")
{
    TEST_CASE("maps_specified_input_argument_by_a_given_function_while_forwarding_others")
    {
        auto e = burst::only<1>([] (auto x) {return std::to_string(x);}) | burst::make_tuple;
        auto t = e(0, 1, 2);
        CHECK(t == std::make_tuple(0, "1", 2));
    }

    TEST_CASE("may_be_composed_with_another_only")
    {
        const auto square = [] (auto x) {return x * x;};
        const auto cube = [] (auto x) {return x * x * x;};
        auto e = burst::only<1>(square) | burst::only<2>(cube) | burst::sum;
        auto r = e(1, 2, 3);
        CHECK(r == 1 + 4 + 27);
    }

    TEST_CASE("composition_is_associative")
    {
        const auto square = [] (auto x) {return x * x;};
        const auto cube = [] (auto x) {return x * x * x;};

        auto left_grouped = (burst::only<1>(square) | burst::only<2>(cube)) | burst::sum;
        auto right_grouped = burst::only<1>(square) | (burst::only<2>(cube) | burst::sum);

        CHECK(left_grouped(2, 3, 4) == right_grouped(2, 3, 4));
    }

    struct doubler
    {
        explicit doubler (std::size_t & calls):
            calls(calls)
        {
        }

        template <typename T>
        auto operator () (T t) const
        {
            ++calls;
            return t * t;
        }

        std::size_t & calls;
    };

    TEST_CASE("calls_inner_function_once")
    {
        auto calls = std::size_t{0};
        auto e = burst::only<2>(doubler{calls}) | burst::make_tuple;

        e(4, 'a', 3.14);

        CHECK(calls == 1);
    }

    TEST_CASE("every_passed_function_is_stored_inside")
    {
        const auto old_instances_count = dummy::instances_count;

        auto e = burst::only<0>(dummy{}) | burst::only<1>(dummy{});
        static_cast<void>(e);

        CHECK(dummy::instances_count == old_instances_count + 2);
    }

    TEST_CASE("function_is_not_stored_if_passed_by_ref")
    {
        auto d = dummy{};
        const auto old_instances_count = dummy::instances_count;

        auto e = burst::only<5>(std::ref(d));
        static_cast<void>(e);

        CHECK(dummy::instances_count == old_instances_count);
    }

    TEST_CASE("stored_function_invokes_as_const_lvalue_if_only_is_const_lvalue")
    {
        auto calls = std::size_t{0};
        const auto only =
            burst::only<1>(utility::const_lvalue_call_counter(calls)) | burst::make_tuple;

        only(1, "qwe");

        CHECK(calls == 1);
    }

    TEST_CASE("stored_function_invokes_as_lvalue_if_only_is_lvalue")
    {
        auto calls = std::size_t{0};
        auto only = burst::only<0>(utility::lvalue_call_counter(calls)) | burst::make_tuple;

        only(1, "qwe");

        CHECK(calls == 1);
    }

    TEST_CASE("stored_function_invokes_as_rvalue_if_only_is_rvalue")
    {
        auto calls = std::size_t{0};
        (burst::only<1>(utility::rvalue_call_counter(calls)) | burst::make_tuple)(1, "qwe");

        CHECK(calls == 1);
    }

    TEST_CASE("composed_function_invokes_as_rvalue_when_only_is_rvalue")
    {
        auto calls = std::size_t{0};
        (burst::only<2>([] (auto x) {return x + x;}) | utility::rvalue_call_counter(calls))(1, 2, 3);

        CHECK(calls == 1);
    }

    TEST_CASE("referenced_function_invokes_as_lvalue")
    {
        auto calls = std::size_t{0};
        auto c = utility::lvalue_call_counter(calls);
        const auto only = burst::only<1>(std::ref(c)) | burst::make_tuple;

        only(1, "qwe");

        CHECK(calls == 1);
    }

    TEST_CASE("const_referenced_function_invokes_as_const_lvalue")
    {
        auto calls = std::size_t{0};
        const auto c = utility::const_lvalue_call_counter(calls);
        const auto only = burst::only<2>(std::ref(c)) | burst::make_tuple;

        only(1, "qwe", 3.14);

        CHECK(calls == 1);
    }

    TEST_CASE("is_a_constexpr_function")
    {
        constexpr auto e = burst::only<2>(&burst::intlog2<int>) | burst::make_tuple;
        constexpr auto t = e(1, 256, 1024);
        CHECK(t == std::make_tuple(1, 256, 10));
    }
}
