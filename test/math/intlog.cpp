#include <burst/math/intlog.hpp>

#include <boost/test/unit_test.hpp>

#include <limits>

BOOST_AUTO_TEST_SUITE(intlog)
    BOOST_AUTO_TEST_CASE(throws_beyond_the_domain)
    {
        BOOST_CHECK_THROW(burst::intlog(0ul, 10ul), std::logic_error);
    }

    BOOST_AUTO_TEST_CASE(throws_on_non_positive_base)
    {
        BOOST_CHECK_THROW(burst::intlog(10ul, 0ul), std::logic_error);
    }

    BOOST_AUTO_TEST_CASE(throws_on_base_equal_to_one)
    {
        BOOST_CHECK_THROW(burst::intlog(10ul, 1ul), std::logic_error);
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

    BOOST_AUTO_TEST_CASE(accepts_signed_values)
    {
        BOOST_CHECK_EQUAL(burst::intlog(13, 3), 2);
    }
BOOST_AUTO_TEST_SUITE_END()
