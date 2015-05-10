#include <limits>

#include <boost/test/unit_test.hpp>

#include <burst/math/intlog2.hpp>

BOOST_AUTO_TEST_SUITE(intlog2)
    BOOST_AUTO_TEST_CASE(intlog2_of_zero_is_maximal_value_of_integer)
    {
        BOOST_CHECK_EQUAL(burst::intlog2(0ul), std::numeric_limits<unsigned long>::max());
    }

    BOOST_AUTO_TEST_CASE(intlog2_of_one_is_zero)
    {
        BOOST_CHECK_EQUAL(burst::intlog2(1u), 0);
    }

    BOOST_AUTO_TEST_CASE(intlog2_of_power_of_two_is_exactly_that_power)
    {
        BOOST_CHECK_EQUAL(burst::intlog2(8ul), 3);
    }

    BOOST_AUTO_TEST_CASE(intlog2_of_non_power_of_two_is_integer_part_of_logarithm)
    {
        BOOST_CHECK_EQUAL(burst::intlog2(15ul), 3);
    }

    BOOST_AUTO_TEST_CASE(intlog2_is_a_constexpr_function)
    {
        constexpr auto x = burst::intlog2(100ul);
        BOOST_CHECK_EQUAL(x, 6);
    }
BOOST_AUTO_TEST_SUITE_END()
