#include <burst/algorithm/sum.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(sum)
    BOOST_AUTO_TEST_CASE(sum_is_a_constexpr_function)
    {
        constexpr auto x = burst::sum({1.62, 2.72, 3.14});
        BOOST_CHECK_CLOSE(x, 7.48, 0.0001);
    }

    BOOST_AUTO_TEST_CASE(sum_of_one_element_equals_that_element)
    {
        BOOST_CHECK_EQUAL(burst::sum({-1}), -1);
    }

    BOOST_AUTO_TEST_CASE(sum_of_empty_list_is_zero)
    {
        auto empty = std::initializer_list<std::size_t>{};
        BOOST_CHECK_EQUAL(burst::sum(empty), 0ul);
    }

    BOOST_AUTO_TEST_CASE(accepts_variadic)
    {
        constexpr auto x = burst::sum(1, 2, 3);
        BOOST_CHECK_EQUAL(x, 6);
    }
BOOST_AUTO_TEST_SUITE_END()
