#include <burst/integer/left_shift.hpp>

#include <boost/test/unit_test.hpp>

#include <type_traits>

BOOST_AUTO_TEST_SUITE(left_shift)
    BOOST_AUTO_TEST_CASE(return_value_type_is_same_to_the_type_of_the_shifted_value)
    {
        {
            using result_type = decltype(burst::left_shift(std::int8_t{2}, std::int64_t{5}));
            BOOST_CHECK((std::is_same<result_type, std::int8_t>::value));
        }
        {
            using result_type = decltype(burst::left_shift(std::uint16_t{2}, std::uint8_t{5}));
            BOOST_CHECK((std::is_same<result_type, std::uint16_t>::value));
        }
    }

    BOOST_AUTO_TEST_CASE(is_constexpr_function)
    {
        constexpr auto value = burst::left_shift(0b101, 1);
        BOOST_CHECK_EQUAL(value, 0b1010);
    }
BOOST_AUTO_TEST_SUITE_END()
