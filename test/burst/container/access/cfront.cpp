#include <burst/container/access/cfront.hpp>
#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>

#include <doctest/doctest.h>

#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>

TEST_SUITE("cfront")
{
    TEST_CASE("always_returns_const_reference_of_container")
    {
        auto integers = burst::make_vector({1, 2, 3});
        static_assert(std::is_same<decltype(burst::cfront(integers)), const int &>::value, "");

        const auto characters = burst::make_vector({'a', 'b', 'c'});
        static_assert(std::is_same<decltype(burst::cfront(characters)), const char &>::value, "");
    }

    TEST_CASE("returns_exactly_container_front")
    {
        auto reals = burst::make_list({1.6, 3.14, 2.7});
        CHECK(std::addressof(burst::cfront(reals)) == std::addressof(reals.front()));
    }

    TEST_CASE("always_returns_const_reference_of_array")
    {
        std::string strings[] = {"123", "456", "789"};
        static_assert(std::is_same<decltype(burst::cfront(strings)), const std::string &>::value, "");

        const std::size_t integers[] = {12, 34, 56};
        static_assert(std::is_same<decltype(burst::cfront(integers)), const std::size_t &>::value, "");
    }

    TEST_CASE("returns_address_of_array")
    {
        int array[] = {1, 2, 3};
        CHECK(std::addressof(burst::cfront(array)) == array);
    }

    TEST_CASE("is_a_functional_object")
    {
        const auto v = burst::make_vector({1, 2, 3});
        const auto cf = burst::cfront;

        CHECK(cf(v) == 1);
    }
}
