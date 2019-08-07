#include <burst/tuple/apply.hpp>
#include <utility/caller_dummies.hpp>

#include <doctest/doctest.h>

#include <cstddef>
#include <functional>
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

TEST_SUITE("apply")
{
    TEST_CASE("applies_function_to_elements_of_unwrapped_tuple")
    {
        auto a = burst::apply(std::plus<>{});
        auto r = a(std::make_tuple(1, 2));
        CHECK(r == 3);
    }

    TEST_CASE("passed_function_is_stored_inside")
    {
        const auto old_instances_count = dummy::instances_count;

        auto a = burst::apply(dummy{});
        static_cast<void>(a);

        CHECK(dummy::instances_count == old_instances_count + 1);
    }

    TEST_CASE("function_is_not_stored_if_passed_by_ref")
    {
        auto d = dummy{};
        const auto old_instances_count = dummy::instances_count;

        auto a = burst::apply(std::ref(d));
        static_cast<void>(a);

        CHECK(dummy::instances_count == old_instances_count);
    }

    TEST_CASE("stored_function_invokes_as_const_lvalue_when_apply_is_const_lvalue")
    {
        auto calls = std::size_t{0};
        const auto a = burst::apply(utility::const_lvalue_call_counter(calls));

        a(std::make_tuple(1, "qwe"));

        CHECK(calls == 1);
    }

    TEST_CASE("stored_function_invokes_as_lvalue_when_apply_is_lvalue")
    {
        auto calls = std::size_t{0};
        auto a = burst::apply(utility::lvalue_call_counter(calls));

        a(std::make_tuple(1, "qwe"));

        CHECK(calls == 1);
    }

    TEST_CASE("stored_function_invokes_as_rvalue_when_apply_is_rvalue")
    {
        auto calls = std::size_t{0};
        auto c = utility::rvalue_call_counter(calls);

        burst::apply(c)(std::make_tuple(1, "qwe"));

        CHECK(calls == 1);
    }

    TEST_CASE("referenced_function_invokes_as_lvalue")
    {
        auto calls = std::size_t{0};
        auto f = utility::lvalue_call_counter(calls);

        burst::apply(std::ref(f))(std::make_tuple(1, "qwe"));

        CHECK(calls == 1);
    }

    TEST_CASE("const_referenced_function_invokes_as_const_lvalue")
    {
        auto calls = std::size_t{0};
        const auto f = utility::const_lvalue_call_counter(calls);

        burst::apply(std::ref(f))(std::make_tuple(1, "qwe"));

        CHECK(calls == 1);
    }

    TEST_CASE("is_a_constexpr_function")
    {
        constexpr auto a = burst::apply(std::multiplies<>{});
        constexpr auto r = a(std::make_tuple(4, 5));
        CHECK(r == 20);
    }
}
