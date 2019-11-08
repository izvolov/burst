#include <burst/container/access/back.hpp>
#include <burst/container/make_vector.hpp>

#include <boost/test/unit_test.hpp>

#include <memory>
#include <string>
#include <type_traits>

BOOST_AUTO_TEST_SUITE(back)
    BOOST_AUTO_TEST_CASE(returns_a_mutable_reference_of_a_mutable_container)
    {
        auto v = burst::make_vector({1, 2, 3});
        static_assert(std::is_same<decltype(burst::back(v)), int &>::value, "");
    }

    BOOST_AUTO_TEST_CASE(returns_a_const_reference_of_a_const_container)
    {
        const auto v = burst::make_vector({1, 2, 3});
        static_assert(std::is_same<decltype(burst::back(v)), const int &>::value, "");
    }

    BOOST_AUTO_TEST_CASE(returns_exactly_container_back)
    {
        auto v = burst::make_vector({1, 2, 3});
        BOOST_CHECK_EQUAL(std::addressof(burst::back(v)), std::addressof(v.back()));

        burst::back(v) = 7;
        BOOST_CHECK_EQUAL(v.back(), 7);
    }

    BOOST_AUTO_TEST_CASE(returns_a_mutable_reference_of_a_mutable_array)
    {
        std::string strings[] = {"123", "456", "789"};
        static_assert(std::is_same<decltype(burst::back(strings)), std::string &>::value, "");
    }

    BOOST_AUTO_TEST_CASE(returns_a_const_reference_of_a_const_array)
    {
        const std::string strings[] = {"123", "456", "789"};
        static_assert(std::is_same<decltype(burst::back(strings)), const std::string &>::value, "");
    }

    BOOST_AUTO_TEST_CASE(returns_exactly_last_array_element)
    {
        char array[] = {'a', 'b', 'c'};
        BOOST_CHECK_EQUAL(std::addressof(burst::back(array)), array + 2);

        burst::back(array) = 'q';
        BOOST_CHECK_EQUAL(array[2], 'q');
    }

    BOOST_AUTO_TEST_CASE(is_a_functional_object)
    {
        const auto v = burst::make_vector({1, 2, 3});
        const auto b = burst::back;

        BOOST_CHECK_EQUAL(b(v), 3);
    }
BOOST_AUTO_TEST_SUITE_END()
