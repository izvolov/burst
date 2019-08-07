#include <burst/algorithm/sum.hpp>
#include <burst/functional/each.hpp>
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

TEST_SUITE("each")
{
    TEST_CASE("forwards_input_arguments_mapped_by_specified_function")
    {
        auto e = burst::each([] (auto x) {return x + x;}) | burst::make_tuple;
        auto t = e(4, std::string("qwe"));
        CHECK(t == std::make_tuple(8, "qweqwe"));
    }

    TEST_CASE("may_be_composed_with_another_each")
    {
        const auto square = [] (auto x) {return x * x;};
        auto e = burst::each(square) | burst::each(square) | burst::sum;
        auto r = e(1, 2, 3);
        CHECK(r == 1 + 16 + 81);
    }

    TEST_CASE("composition_may_be_composed_again")
    {
        const auto square = [] (auto x) {return x * x;};
        auto e = burst::each(square) | burst::sum | square;
        auto r = e(1, 2, 3);
        CHECK(r == (1 + 4 + 9) * (1 + 4 + 9));
    }

    TEST_CASE("calls_inner_function_on_every_input_element")
    {
        auto calls = std::size_t{0};
        auto e = burst::each(doubler{calls}) | burst::make_tuple;

        e(4, 'a', 3.14);

        CHECK(calls == 3);
    }

    TEST_CASE("every_passed_function_is_stored_inside")
    {
        const auto old_instances_count = dummy::instances_count;

        auto e = burst::each(dummy{}) | burst::each(dummy{});
        static_cast<void>(e);

        CHECK(dummy::instances_count == old_instances_count + 2);
    }

    TEST_CASE("function_is_not_stored_if_passed_by_ref")
    {
        auto d = dummy{};
        const auto old_instances_count = dummy::instances_count;

        auto e = burst::each(std::ref(d));
        static_cast<void>(e);

        CHECK(dummy::instances_count == old_instances_count);
    }

    TEST_CASE("stored_function_invokes_as_const_lvalue_if_each_is_const_lvalue")
    {
        auto calls = std::size_t{0};
        const auto each =
            burst::each(utility::const_lvalue_call_counter(calls)) | burst::make_tuple;

        each(1, "qwe");

        CHECK(calls == 2);
    }

    TEST_CASE("stored_function_invokes_as_lvalue_if_each_is_lvalue")
    {
        auto calls = std::size_t{0};
        auto each = burst::each(utility::lvalue_call_counter(calls)) | burst::make_tuple;

        each(1, "qwe");

        CHECK(calls == 2);
    }

    TEST_CASE("composed_function_invokes_as_rvalue_when_each_is_rvalue")
    {
        auto calls = std::size_t{0};
        (burst::each([] (auto x) {return x + x;}) | utility::rvalue_call_counter(calls))(1, 2, 3);

        CHECK(calls == 1);
    }

    TEST_CASE("stored_function_invokes_as_lvalue_if_each_is_rvalue")
    {
        auto calls = std::size_t{0};
        (burst::each(utility::lvalue_call_counter(calls)) | burst::make_tuple)(1, "qwe");

        CHECK(calls == 2);
    }

    TEST_CASE("referenced_function_invokes_as_lvalue")
    {
        auto calls = std::size_t{0};
        auto c = utility::lvalue_call_counter(calls);
        const auto each = burst::each(std::ref(c)) | burst::make_tuple;

        each(1, "qwe");

        CHECK(calls == 2);
    }

    TEST_CASE("const_referenced_function_invokes_as_const_lvalue")
    {
        auto calls = std::size_t{0};
        const auto c = utility::const_lvalue_call_counter(calls);
        const auto each = burst::each(std::ref(c)) | burst::make_tuple;

        each(1, "qwe");

        CHECK(calls == 2);
    }

    TEST_CASE("is_a_constexpr_function")
    {
        constexpr auto e = burst::each(&burst::intlog2<int>) | burst::make_tuple;
        constexpr auto t = e(1, 256, 1024);
        CHECK(t == std::make_tuple(0, 8, 10));
    }

    TEST_CASE("is_composable_with_only")
    {
        const auto cube = [] (auto x) {return x * x * x;};
        const auto stringify = [] (auto x) {return std::to_string(x);};
        const auto twice = [] (auto x) {return x + x;};
        const auto size = [] (const auto & s) {return s.size();};

        const auto f =
            burst::only<1>(cube) |
            burst::each(stringify) |
            burst::only<0>(twice) |
            burst::each(size) |
            burst::sum;

        CHECK(f(10, 20, 30) == 10);
    }
}
