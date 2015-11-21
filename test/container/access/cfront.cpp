#include <boost/test/unit_test.hpp>

#include <burst/container/access/cfront.hpp>
#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>

BOOST_AUTO_TEST_SUITE(cfront)
    BOOST_AUTO_TEST_CASE(always_returns_const_reference_of_container)
    {
        auto integers = burst::make_vector({1, 2, 3});
        static_assert(std::is_same<decltype(burst::cfront(integers)), const int &>::value, "");

        const auto characters = burst::make_vector({'a', 'b', 'c'});
        static_assert(std::is_same<decltype(burst::cfront(characters)), const char &>::value, "");
    }

    BOOST_AUTO_TEST_CASE(returns_exactly_container_front)
    {
        auto reals = burst::make_list({1.6, 3.14, 2.7});
        BOOST_CHECK_EQUAL(std::addressof(burst::cfront(reals)), std::addressof(reals.front()));
    }

    BOOST_AUTO_TEST_CASE(always_returns_const_reference_of_array)
    {
        std::string strings[] = {"123", "456", "789"};
        static_assert(std::is_same<decltype(burst::cfront(strings)), const std::string &>::value, "");

        const std::size_t integers[] = {12, 34, 56};
        static_assert(std::is_same<decltype(burst::cfront(integers)), const std::size_t &>::value, "");
    }

    BOOST_AUTO_TEST_CASE(returns_address_of_array)
    {
        int array[] = {1, 2, 3};
        BOOST_CHECK_EQUAL(std::addressof(burst::cfront(array)), array);
    }
BOOST_AUTO_TEST_SUITE_END()
