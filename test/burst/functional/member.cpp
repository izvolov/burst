#include <burst/functional/member.hpp>

#include <doctest/doctest.h>

#include <type_traits>

namespace // anonymous
{
    struct dummy
    {
        int member;
    };
} // namespace anonymous

TEST_SUITE("member_macro")
{
    TEST_CASE("generates_a_function_object_returning_member_of_a_class")
    {
        auto get_member = BURST_MEMBER(member);
        CHECK(get_member(dummy{17}) == 17);
    }

    TEST_CASE("is_composable")
    {
        const auto square_of_member = BURST_MEMBER(member) | [] (auto x) {return x * x;};
        const auto d = dummy{5};
        CHECK(square_of_member(d) == 25);
    }

    TEST_CASE("returns_a_const_reference_if_object_is_const")
    {
        const auto member_of = BURST_MEMBER(member);
        const auto d = dummy{42};

        decltype(auto) m = member_of(d);

        CHECK(std::is_same<decltype(m), const int &>::value);
    }

    TEST_CASE("returns_a_mutable_reference_if_object_is_mutable")
    {
        const auto member_of = BURST_MEMBER(member);
        auto d = dummy{42};

        member_of(d) = 17;

        CHECK(d.member == 17);
    }

    TEST_CASE("returns_rvalue_if_object_is_rvalue")
    {
        const auto member_of = BURST_MEMBER(member);
        CHECK(std::is_same<decltype(member_of(dummy{5})), int &&>::value);
    }
}
