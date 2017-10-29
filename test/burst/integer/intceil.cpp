#include <burst/integer/intceil.hpp>

#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

BOOST_AUTO_TEST_SUITE(intceil)
    BOOST_AUTO_TEST_CASE(returns_first_number_not_less_than_initial_which_is_a_multiple_of_factor)
    {
        BOOST_CHECK_EQUAL(burst::intceil(13, 5), 15);
        BOOST_CHECK_EQUAL(burst::intceil(12u, 5u), 15u);
    }

    BOOST_AUTO_TEST_CASE(intceil_of_number_which_is_already_a_multiple_of_factor_is_itself)
    {
        BOOST_CHECK_EQUAL(burst::intceil(36u, 6), 36);
        BOOST_CHECK_EQUAL(burst::intceil(25, 5), 25);
    }

    BOOST_AUTO_TEST_CASE(intceil_of_0_is_always_0)
    {
        BOOST_CHECK_EQUAL(burst::intceil(0, 1), 0);
        BOOST_CHECK_EQUAL(burst::intceil(0, 100500), 0);
    }

    BOOST_AUTO_TEST_CASE(negative_numbers_rounds_toward_zero)
    {
        BOOST_CHECK_EQUAL(burst::intceil(-4, 3), -3);
        BOOST_CHECK_EQUAL(burst::intceil(-3, 3), -3);
        BOOST_CHECK_EQUAL(burst::intceil(-2, 3), 0);
        BOOST_CHECK_EQUAL(burst::intceil(-1, 3), 0);
    }

    BOOST_AUTO_TEST_CASE(intceil_of_a_number_by_1_is_the_same_number)
    {
        BOOST_CHECK_EQUAL(burst::intceil(15, 1), 15);
        BOOST_CHECK_EQUAL(burst::intceil(-3, 1), -3);
    }

    BOOST_AUTO_TEST_CASE(throws_on_non_positive_factor)
    {
        BOOST_CHECK_THROW(burst::intceil(10u, 0ul), std::domain_error);
        BOOST_CHECK_THROW(burst::intceil(10ul, -1), std::domain_error);
    }

    BOOST_AUTO_TEST_CASE(is_a_constexpr_function)
    {
        constexpr auto x = burst::intceil(21ul, 2ul);
        BOOST_CHECK_EQUAL(x, 22);
    }

    BOOST_AUTO_TEST_CASE(accepts_different_argument_types)
    {
        constexpr auto int32_max = std::numeric_limits<std::int32_t>::max();
        BOOST_CHECK_EQUAL(burst::intceil(int32_max - 1, std::uint64_t{int32_max}), int32_max);
    }

    BOOST_AUTO_TEST_CASE(return_value_type_is_type_of_first_argument)
    {
        using first_argument_type = std::uint32_t;
        using second_argument_type = std::int64_t;
        static_assert(not std::is_same<first_argument_type, second_argument_type>::value, "");

        using result_type = decltype(burst::intceil(first_argument_type{2}, second_argument_type{5}));

        BOOST_CHECK((std::is_same<result_type, first_argument_type>::value));
    }
BOOST_AUTO_TEST_SUITE_END()
