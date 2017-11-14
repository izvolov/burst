#include <burst/integer/divceil.hpp>

#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

BOOST_AUTO_TEST_SUITE(divceil)
    BOOST_AUTO_TEST_CASE(returns_ceiled_integral_part_of_division)
    {
        BOOST_CHECK_EQUAL(burst::divceil(13, 5), 3);
        BOOST_CHECK_EQUAL(burst::divceil(12u, 5u), 3u);
    }

    BOOST_AUTO_TEST_CASE(negative_numbers_rounds_toward_zero)
    {
        BOOST_CHECK_EQUAL(burst::divceil(-4, 3), -1);
        BOOST_CHECK_EQUAL(burst::divceil(-3, 3), -1);
        BOOST_CHECK_EQUAL(burst::divceil(-2, 3), 0);
        BOOST_CHECK_EQUAL(burst::divceil(-1, 3), 0);
    }

    BOOST_AUTO_TEST_CASE(divceil_of_a_multiple_is_equal_to_the_result_of_an_integral_division)
    {
        BOOST_CHECK_EQUAL(burst::divceil(36u, 6), 6);
        BOOST_CHECK_EQUAL(burst::divceil(25, 5), 5);
        BOOST_CHECK_EQUAL(burst::divceil(-16, 4u), -4);
    }

    BOOST_AUTO_TEST_CASE(divceil_of_0_is_always_0)
    {
        BOOST_CHECK_EQUAL(burst::divceil(0, 1), 0);
        BOOST_CHECK_EQUAL(burst::divceil(0u, 100500), 0);
    }

    BOOST_AUTO_TEST_CASE(undefined_on_non_positive_divisor)
    {
        BOOST_CHECK_THROW(burst::divceil(10u, 0ul), std::domain_error);
        BOOST_CHECK_THROW(burst::divceil(10ul, -1), std::domain_error);
    }

    BOOST_AUTO_TEST_CASE(divceil_of_a_number_by_1_is_the_same_number)
    {
        BOOST_CHECK_EQUAL(burst::divceil(15, 1), 15);
        BOOST_CHECK_EQUAL(burst::divceil(-3, 1), -3);
    }

    BOOST_AUTO_TEST_CASE(division_by_greater_in_magnitude_divisor_is_one_for_positive_dividend)
    {
        BOOST_CHECK_EQUAL(burst::divceil(5, 6), 1);
    }

    BOOST_AUTO_TEST_CASE(division_by_greater_in_magnitude_divisor_is_zero_for_non_positive_dividend)
    {
        BOOST_CHECK_EQUAL(burst::divceil(-5, 6), 0);
    }

    BOOST_AUTO_TEST_CASE(is_a_constexpr_function)
    {
        constexpr auto x = burst::divceil(21ul, 2ul);
        BOOST_CHECK_EQUAL(x, 11);
    }

    BOOST_AUTO_TEST_CASE(does_not_overflow)
    {
        constexpr auto int32_max = std::numeric_limits<std::int32_t>::max();
        constexpr auto int32_min = std::numeric_limits<std::int32_t>::min();
        constexpr auto uint32_max = std::numeric_limits<std::uint32_t>::max();

        BOOST_CHECK_EQUAL(burst::divceil(uint32_max, uint32_max), 1);
        BOOST_CHECK_EQUAL(burst::divceil(uint32_max, 10), 429496730);
        BOOST_CHECK_EQUAL(burst::divceil(int32_max, int32_max), 1);
        BOOST_CHECK_EQUAL(burst::divceil(int32_max, uint32_max), 1);
        BOOST_CHECK_EQUAL(burst::divceil(int32_max, 10), 214748365);
        BOOST_CHECK_EQUAL(burst::divceil(0u, 10), 0);
        BOOST_CHECK_EQUAL(burst::divceil(0u, uint32_max), 0);
        BOOST_CHECK_EQUAL(burst::divceil(int32_min, 10), -214748364);
        BOOST_CHECK_EQUAL(burst::divceil(int32_min, uint32_max), 0);
    }

    BOOST_AUTO_TEST_CASE(accepts_different_argument_types)
    {
        constexpr auto int32_max = std::numeric_limits<std::int32_t>::max();
        BOOST_CHECK_EQUAL(burst::divceil(int32_max - 1, std::uint64_t{int32_max}), 1);
        BOOST_CHECK_EQUAL(burst::divceil(int32_max - 1, std::uint32_t{int32_max}), 1);
        BOOST_CHECK_EQUAL(burst::divceil(std::int16_t{-1}, std::uint32_t{256}), 0);
        BOOST_CHECK_EQUAL(burst::divceil(std::int32_t{-1}, std::uint16_t{256}), 0);
        BOOST_CHECK_EQUAL(burst::divceil(std::uint32_t{5}, std::int32_t{2}), 3);
        BOOST_CHECK_EQUAL(burst::divceil(std::uint32_t{5}, std::int64_t{2}), 3);
        BOOST_CHECK_EQUAL(burst::divceil(std::uint32_t{5}, std::int32_t{256}), 1);
        BOOST_CHECK_EQUAL(burst::divceil(std::uint32_t{5}, std::int64_t{256}), 1);
        BOOST_CHECK_EQUAL(burst::divceil(std::uint32_t{0}, std::int32_t{256}), 0);
        BOOST_CHECK_EQUAL(burst::divceil(std::uint32_t{0}, std::int64_t{256}), 0);
        BOOST_CHECK_EQUAL(burst::divceil(std::int8_t{0}, std::uint8_t{25}), 0);
        BOOST_CHECK_EQUAL(burst::divceil(std::uint16_t{5}, std::int16_t{25}), 1);
    }

    BOOST_AUTO_TEST_CASE(return_value_type_is_type_of_first_argument)
    {
        using first_result_type = decltype(burst::divceil(std::uint32_t{2}, std::int64_t{5}));
        BOOST_CHECK((std::is_same<first_result_type, std::uint32_t>::value));
        using second_result_type = decltype(burst::divceil(std::uint32_t{2}, std::int32_t{5}));
        BOOST_CHECK((std::is_same<second_result_type, std::uint32_t>::value));
        using third_result_type = decltype(burst::divceil(std::uint32_t{2}, std::uint64_t{5}));
        BOOST_CHECK((std::is_same<third_result_type, std::uint32_t>::value));
        using fourth_result_type = decltype(burst::divceil(std::uint32_t{2}, std::int16_t{5}));
        BOOST_CHECK((std::is_same<fourth_result_type, std::uint32_t>::value));
    }
BOOST_AUTO_TEST_SUITE_END()
