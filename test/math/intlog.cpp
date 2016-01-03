#include <burst/math/intlog.hpp>

#include <boost/test/unit_test.hpp>

#include <limits>

BOOST_AUTO_TEST_SUITE(intlog)
    BOOST_AUTO_TEST_CASE(any_base_intlog_of_zero_is_maximal_value_of_integer)
    {
        BOOST_CHECK_EQUAL(burst::intlog(0ul, 10ul), std::numeric_limits<unsigned long>::max());
    }

    BOOST_AUTO_TEST_CASE(any_base_intlog_of_one_is_zero)
    {
        BOOST_CHECK_EQUAL(burst::intlog(1u, 100500u), 0);
    }

    BOOST_AUTO_TEST_CASE(intlog_of_power_of_its_base_is_exactly_that_power)
    {
        BOOST_CHECK_EQUAL(burst::intlog(100ul, 10ul), 2);
    }

    BOOST_AUTO_TEST_CASE(intlog_of_non_power_of_its_base_is_integer_part_of_logarithm)
    {
        BOOST_CHECK_EQUAL(burst::intlog(30ul, 3ul), 3);
    }

    BOOST_AUTO_TEST_CASE(intlog_is_a_constexpr_function)
    {
        constexpr auto x = burst::intlog(20ul, 2ul);
        BOOST_CHECK_EQUAL(x, 4);
    }
BOOST_AUTO_TEST_SUITE_END()
