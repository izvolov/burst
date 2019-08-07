#include <burst/functional/mem_fn.hpp>

#include <doctest/doctest.h>

#include <type_traits>

namespace // anonymous
{
    struct dummy
    {
        const int & member_function () const
        {
            return x;
        }

        int & member_function ()
        {
            return x;
        }

        int member_function (int y) const
        {
            return x + y;
        }

        int x;
    };
} // namespace anonymous

TEST_SUITE("mem_fn_macro")
{
    TEST_CASE("generates_a_function_object_calling_member_function_of_a_class")
    {
        auto call_mem_fn = BURST_MEM_FN(member_function);
        CHECK(call_mem_fn(dummy{17}) == 17);
    }

    TEST_CASE("is_composable")
    {
        const auto square_of_mem_fn = BURST_MEM_FN(member_function) | [] (auto x) {return x * x;};
        const auto d = dummy{5};
        CHECK(square_of_mem_fn(d) == 25);
    }

    TEST_CASE("returns_a_const_reference_if_object_is_const")
    {
        const auto mem_fn_of = BURST_MEM_FN(member_function);
        const auto d = dummy{42};

        decltype(auto) m = mem_fn_of(d);

        CHECK(std::is_same<decltype(m), const int &>::value);
    }

    TEST_CASE("returns_a_non_const_reference_if_object_is_non_const")
    {
        const auto mem_fn_of = BURST_MEM_FN(member_function);
        auto d = dummy{42};

        mem_fn_of(d) = 17;

        CHECK(d.member_function() == 17);
    }

    TEST_CASE("allows_overloads")
    {
        const auto mem_fn_of = BURST_MEM_FN(member_function);
        const auto d = dummy{3};
        CHECK(mem_fn_of(d, 4) == 7);
    }
}
