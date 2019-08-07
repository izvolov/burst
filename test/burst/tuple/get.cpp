#include <burst/tuple/get.hpp>

#include <doctest/doctest.h>
#include <boost/tuple/tuple.hpp>

#include <tuple>
#include <type_traits>
#include <utility>

TEST_SUITE("get")
{
    TEST_CASE("gets_nth_element_of_a_tuple")
    {
        CHECK(burst::get<0>(std::make_tuple('a', 'b', 'c')) == 'a');
    }

    TEST_CASE("is_a_function_object")
    {
        auto g = burst::get<0>;
        CHECK(g(std::make_tuple('a', 'b', 'c')) == 'a');
    }

    TEST_CASE("element_of_lvalue_tuple_is_lvalue")
    {
        auto t = std::make_tuple(42, "qwe", 3.14);

        burst::get<0>(t) = 17;

        CHECK(std::get<0>(t) == 17);
    }

    TEST_CASE("element_of_const_lvalue_tuple_is_const_lvalue")
    {
        const auto t = std::make_tuple(42, "qwe", 3.14);

        using element_type = std::tuple_element<0, decltype(t)>::type;
        using result_type = decltype(burst::get<0>(t));

        CHECK((std::is_same<result_type, const element_type &>::value));
    }

    TEST_CASE("element_of_rvalue_tuple_is_rvalue_reference")
    {
        CHECK
        (
            std::is_rvalue_reference
            <
                decltype(burst::get<0>(std::make_tuple(42, "qwe", 3.14)))
            >
            ::value
        );
    }

    TEST_CASE("is_a_constexpr_function")
    {
        constexpr auto r = burst::get<1>(std::make_tuple(0, 11, 222));
        CHECK(r == 11);
    }

    TEST_CASE("is_compatible_with_pair")
    {
        const auto p = std::make_pair('c', 3.14);
        CHECK(burst::get<0>(p) == 'c');
    }

    TEST_CASE("is_compatible_with_boost_tuple")
    {
        const auto t = boost::make_tuple(42, 3.14, true);
        CHECK(burst::get<2>(t) == true);
    }
}
