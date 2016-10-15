#include <burst/tuple/get.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/tuple/tuple.hpp>

#include <tuple>
#include <type_traits>
#include <utility>

BOOST_AUTO_TEST_SUITE(get)
    BOOST_AUTO_TEST_CASE(gets_nth_element_of_a_tuple)
    {
        BOOST_CHECK_EQUAL(burst::get<0>(std::make_tuple('a', 'b', 'c')), 'a');
    }

    BOOST_AUTO_TEST_CASE(is_a_function_object)
    {
        auto g = burst::get<0>;
        BOOST_CHECK_EQUAL(g(std::make_tuple('a', 'b', 'c')), 'a');
    }

    BOOST_AUTO_TEST_CASE(element_of_lvalue_tuple_is_lvalue)
    {
        auto t = std::make_tuple(42, "qwe", 3.14);

        burst::get<0>(t) = 17;

        BOOST_CHECK_EQUAL(std::get<0>(t), 17);
    }

    BOOST_AUTO_TEST_CASE(element_of_const_lvalue_tuple_is_const_lvalue)
    {
        const auto t = std::make_tuple(42, "qwe", 3.14);

        using element_type = std::tuple_element<0, decltype(t)>::type;
        using result_type = decltype(burst::get<0>(t));

        BOOST_CHECK((std::is_same<result_type, const element_type &>::value));
    }

    BOOST_AUTO_TEST_CASE(element_of_rvalue_tuple_is_rvalue_reference)
    {
        BOOST_CHECK
        (
            std::is_rvalue_reference
            <
                decltype(burst::get<0>(std::make_tuple(42, "qwe", 3.14)))
            >
            ::value
        );
    }

    BOOST_AUTO_TEST_CASE(is_a_constexpr_function)
    {
        constexpr auto r = burst::get<1>(std::make_tuple(0, 11, 222));
        BOOST_CHECK_EQUAL(r, 11);
    }

    BOOST_AUTO_TEST_CASE(is_compatible_with_pair)
    {
        const auto p = std::make_pair('c', 3.14);
        BOOST_CHECK_EQUAL(burst::get<0>(p), 'c');
    }

    BOOST_AUTO_TEST_CASE(is_compatible_with_boost_tuple)
    {
        const auto t = boost::make_tuple(42, 3.14, true);
        BOOST_CHECK_EQUAL(burst::get<2>(t), true);
    }
BOOST_AUTO_TEST_SUITE_END()
