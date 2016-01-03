#include <burst/functional/low_byte.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(low_byte)
    BOOST_AUTO_TEST_CASE(low_byte_has_a_constexpr_parentheses)
    {
        constexpr auto x = burst::low_byte<>()(0xff01);
        static_assert(x == 0x01, "");
    }
BOOST_AUTO_TEST_SUITE_END()
