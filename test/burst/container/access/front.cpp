#include <burst/container/access/front.hpp>
#include <burst/container/make_vector.hpp>

#include <boost/test/unit_test.hpp>

#include <memory>
#include <string>
#include <type_traits>

BOOST_AUTO_TEST_SUITE(front)
    BOOST_AUTO_TEST_CASE(returns_a_mutable_reference_of_a_mutable_container)
    {
        auto v = burst::make_vector({1, 2, 3});
        static_assert(std::is_same<decltype(burst::front(v)), int &>::value, "");
    }

    BOOST_AUTO_TEST_CASE(returns_a_const_reference_of_a_const_container)
    {
        const auto v = burst::make_vector({1, 2, 3});
        static_assert(std::is_same<decltype(burst::front(v)), const int &>::value, "");
    }

    BOOST_AUTO_TEST_CASE(returns_exactly_container_front)
    {
        auto v = burst::make_vector({1, 2, 3});
        BOOST_CHECK_EQUAL(std::addressof(burst::front(v)), std::addressof(v.front()));

        burst::front(v) = 7;
        BOOST_CHECK_EQUAL(v.front(), 7);
    }

    BOOST_AUTO_TEST_CASE(returns_a_mutable_reference_of_a_mutable_array)
    {
        std::string strings[] = {"123", "456", "789"};
        static_assert(std::is_same<decltype(burst::front(strings)), std::string &>::value, "");
    }

    BOOST_AUTO_TEST_CASE(returns_a_const_reference_of_a_const_array)
    {
        const std::string strings[] = {"123", "456", "789"};
        static_assert(std::is_same<decltype(burst::front(strings)), const std::string &>::value, "");
    }

    BOOST_AUTO_TEST_CASE(returns_exactly_first_array_element)
    {
        char array[] = {'a', 'b', 'c'};
        BOOST_CHECK_EQUAL(std::addressof(burst::front(array)), array);

        burst::front(array) = 'q';
        BOOST_CHECK_EQUAL(array[0], 'q');
    }

    BOOST_AUTO_TEST_CASE(is_a_functional_object)
    {
        const auto v = burst::make_vector({1, 2, 3});
        const auto f = burst::front;

        BOOST_CHECK_EQUAL(f(v), 1);
    }
BOOST_AUTO_TEST_SUITE_END()
