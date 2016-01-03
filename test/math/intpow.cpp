#include <burst/math/intpow.hpp>

#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <type_traits>

BOOST_AUTO_TEST_SUITE(intpow)
    BOOST_AUTO_TEST_CASE(any_power_of_zero_is_zero)
    {
        BOOST_CHECK_EQUAL(burst::intpow(0, 1), 0);
        BOOST_CHECK_EQUAL(burst::intpow(0, 2), 0);
        BOOST_CHECK_EQUAL(burst::intpow(0, 10), 0);
        BOOST_CHECK_EQUAL(burst::intpow(0, 100500), 0);
    }

    BOOST_AUTO_TEST_CASE(zero_power_of_zero_is_one)
    {
        BOOST_CHECK_EQUAL(burst::intpow(0, 0), 1);
    }

    BOOST_AUTO_TEST_CASE(any_power_of_one_is_one)
    {
        BOOST_CHECK_EQUAL(burst::intpow(1, 0), 1);
        BOOST_CHECK_EQUAL(burst::intpow(1, 1), 1);
        BOOST_CHECK_EQUAL(burst::intpow(1, 2), 1);
        BOOST_CHECK_EQUAL(burst::intpow(1, 10), 1);
        BOOST_CHECK_EQUAL(burst::intpow(1, 100500), 1);
    }

    BOOST_AUTO_TEST_CASE(power_must_be_non_negative)
    {
        BOOST_CHECK_THROW(burst::intpow(2, -1), std::logic_error);
    }

    BOOST_AUTO_TEST_CASE(base_may_be_negative)
    {
        BOOST_CHECK_EQUAL(burst::intpow(-1, 2), 1);
        BOOST_CHECK_EQUAL(burst::intpow(-3, 3), -27);
    }

    BOOST_AUTO_TEST_CASE(any_base_to_the_power_of_zero_is_one)
    {
        BOOST_CHECK_EQUAL(burst::intpow(2, 0), 1);
        BOOST_CHECK_EQUAL(burst::intpow(-2, 0), 1);
        BOOST_CHECK_EQUAL(burst::intpow(20, 0), 1);
        BOOST_CHECK_EQUAL(burst::intpow(-100500, 0), 1);
    }

    BOOST_AUTO_TEST_CASE(return_type_deferred_as_base_type)
    {
        BOOST_CHECK((std::is_same<decltype(burst::intpow(1, 10)), decltype(1)>::value));
        BOOST_CHECK((std::is_same<decltype(burst::intpow(1l, 10)), decltype(1l)>::value));
        BOOST_CHECK((std::is_same<decltype(burst::intpow(1ll, 10)), decltype(1ll)>::value));
        BOOST_CHECK((std::is_same<decltype(burst::intpow(1u, 10)), decltype(1u)>::value));
        BOOST_CHECK((std::is_same<decltype(burst::intpow(1ul, 10)), decltype(1ul)>::value));
        BOOST_CHECK((std::is_same<decltype(burst::intpow(1ull, 10)), decltype(1ull)>::value));
    }

    BOOST_AUTO_TEST_CASE(power_of_product_is_product_of_powers)
    {
        // (ab) ^ n = (a ^ n)(b ^ n)
        const auto a = 10;
        const auto b = 3;
        const auto n = 4;
        BOOST_CHECK_EQUAL(burst::intpow(a * b, n), burst::intpow(a, n) * burst::intpow(b, n));
    }

    BOOST_AUTO_TEST_CASE(product_of_powers_with_same_base_equals_that_base_to_the_power_of_sum_of_those_powers)
    {
        // (a ^ n)(a ^ m) = a ^ (n + m)
        const auto a = 4;
        const auto n = 3;
        const auto m = 8;
        BOOST_CHECK_EQUAL(burst::intpow(a, n) * burst::intpow(a, m), burst::intpow(a, n + m));
    }

    BOOST_AUTO_TEST_CASE(some_base_in_power_of_n_to_the_power_of_m_equals_that_base_to_the_power_of_product_of_n_and_m)
    {
        // (a ^ n) ^ m = a ^ (nm)
        const auto a = 7;
        const auto n = 10;
        const auto m = 2;
        BOOST_CHECK_EQUAL(burst::intpow(burst::intpow(a, n), m), burst::intpow(a, n * m));
    }

    BOOST_AUTO_TEST_CASE(intpow_is_a_constexpr_function)
    {
        constexpr auto power = burst::intpow(3, 4);
        BOOST_CHECK_EQUAL(power, 81);
    }
BOOST_AUTO_TEST_SUITE_END()
