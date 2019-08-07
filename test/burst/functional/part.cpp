#include <burst/functional/part.hpp>
#include <utility/caller_dummies.hpp>

#include <doctest/doctest.h>

#include <cstddef>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

namespace // anonymous
{
    struct identity
    {
        explicit identity (std::size_t & calls): calls(calls) {}

        template <typename T>
        auto operator () (T t) const
        {
            ++calls;
            return t;
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

    struct single
    {
        single () = default;
        single (const single &) = delete;
        single & operator = (const single &) = delete;
        single (single &&) = delete;
        single & operator = (single &&) = delete;
        ~single () = default;
    };

    struct noncopyable
    {
        noncopyable () = default;
        noncopyable (const noncopyable &) = delete;
        noncopyable & operator = (const noncopyable &) = delete;
        noncopyable (noncopyable &&) = default;
        noncopyable & operator = (noncopyable &&) = default;
        ~noncopyable () = default;
    };
} // namespace anonymous

TEST_SUITE("part")
{
    TEST_CASE("enables_partial_application_of_arbitrary_function_object")
    {
        const auto p = burst::part(std::plus<>{});
        CHECK(p(1, 2) == 3);
    }

    TEST_CASE("partial_application_is_associative")
    {
        const auto l = [] (auto x, auto y, auto z) {return x * y * z;};
        CHECK(burst::part(l, 4, 5)(6) == burst::part(l, 4)(5, 6));
    }

    TEST_CASE("call_for_part_function_does_not_invoke_passed_functional_object")
    {
        std::size_t calls = 0;
        burst::part(identity{calls}, 17);

        CHECK(calls == 0);
    }

    TEST_CASE("for_repetitive_partial_application_one_should_call_part_function_again")
    {
        const auto l = [] (auto x, auto y, auto z) {return x * y * z;};
        const auto one_argument_captured = burst::part(l, 1);
        const auto two_arguments_captured = burst::part(one_argument_captured, 2);
        CHECK(two_arguments_captured(3) == 6);
    }

    TEST_CASE("partially_applied_objects_are_stored_inside")
    {
        const auto old_instances_count = dummy::instances_count;

        auto c = burst::part([] (auto x) {return x;}, dummy{});
        static_cast<void>(c);

        CHECK(dummy::instances_count == old_instances_count + 1);
    }

    TEST_CASE("object_passed_by_reference_wrapper_is_not_stored_inside")
    {
        auto d = dummy{};
        const auto old_instances_count = dummy::instances_count;

        auto c = burst::part([] (auto x) {return x;}, std::cref(d));
        static_cast<void>(c);

        CHECK(dummy::instances_count == old_instances_count);
    }

    TEST_CASE("object_passed_by_reference_wrapped_is_neither_moved_nor_copied")
    {
        single s{};
        auto c = burst::part([] (auto &&) {return 1;}, std::cref(s));
        static_cast<void>(c);
    }

    TEST_CASE("passed_object_is_neither_moved_nor_copied_on_invoking")
    {
        auto c = burst::part([] (auto &&, auto &&) {return 1;});
        auto x = c(single{}, single{});
        static_cast<void>(x);
    }

    TEST_CASE("stored_function_invokes_as_const_lvalue_if_part_is_const_lvalue")
    {
        auto calls = std::size_t{0};
        const auto p = burst::part(utility::const_lvalue_call_counter(calls));

        p("qwe");

        CHECK(calls == 1);
    }

    TEST_CASE("stored_function_invokes_as_lvalue_if_part_is_lvalue")
    {
        auto calls = std::size_t{0};
        auto part = burst::part(utility::lvalue_call_counter(calls));

        part(1);

        CHECK(calls == 1);
    }

    TEST_CASE("stored_function_invokes_as_rvalue_if_part_is_rvalue")
    {
        auto calls = std::size_t{0};
        auto c = utility::rvalue_call_counter(calls);

        burst::part(c)(3.14);

        CHECK(calls == 1);
    }

    TEST_CASE("referenced_function_invokes_as_lvalue")
    {
        auto calls = std::size_t{0};
        auto f = utility::lvalue_call_counter(calls);

        burst::part(std::ref(f))(true);

        CHECK(calls == 1);
    }

    TEST_CASE("const_referenced_function_invokes_as_const_lvalue")
    {
        auto calls = std::size_t{0};
        const auto f = utility::const_lvalue_call_counter(calls);

        burst::part(std::ref(f))('c');

        CHECK(calls == 1);
    }

    TEST_CASE("stored_object_is_never_copied_on_invoking")
    {
        auto c = burst::part([] (auto &&, auto &&) {return true;}, noncopyable{});
        CHECK(c(noncopyable{}));
    }

    TEST_CASE("is_a_constexpr_function")
    {
        constexpr auto c = burst::part(std::multiplies<>{}, 4);
        constexpr auto r = c(5);
        static_assert(r == 20, "");
    }
}
