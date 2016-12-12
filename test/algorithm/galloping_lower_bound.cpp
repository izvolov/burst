#include <burst/algorithm/galloping_lower_bound.hpp>

#include <boost/test/unit_test.hpp>

#include <functional>
#include <list>
#include <string>
#include <vector>

BOOST_AUTO_TEST_SUITE(galloping_lower_bound)
    BOOST_AUTO_TEST_CASE(searching_an_empty_range_results_range_end)
    {
        std::vector<int> empty;

        auto search_result = burst::galloping_lower_bound(empty.begin(), empty.end(), 1);

        BOOST_CHECK(search_result == empty.end());
    }

    BOOST_AUTO_TEST_CASE(searching_a_value_greater_than_all_values_in_a_given_range_results_range_end)
    {
        std::vector<std::size_t> range{1, 2, 3, 4, 5};

        auto search_result = burst::galloping_lower_bound(range.begin(), range.end(), 100500ul);

        BOOST_CHECK(search_result == range.end());
    }

    BOOST_AUTO_TEST_CASE(searching_a_value_less_than_all_values_in_a_given_range_results_range_begin)
    {
        std::list<std::int8_t> range{10, 20, 30, 40};

        auto search_result = burst::galloping_lower_bound(range.begin(), range.end(), 0);

        BOOST_CHECK(search_result == range.begin());
    }

    BOOST_AUTO_TEST_CASE(searching_absent_value_results_iterator_to_first_value_greater_than_desired)
    {
        auto range = {50, 40, 30, 20, 10};

        auto search_result = burst::galloping_lower_bound(range.begin(), range.end(), 33, std::greater<>{});

        BOOST_CHECK(search_result == range.begin() + 2);
        BOOST_CHECK_EQUAL(*search_result, 30);
    }

    BOOST_AUTO_TEST_CASE(searching_existing_value_results_iterator_to_that_value)
    {
        std::vector<std::string> range{"abc", "bc", "c"};

        auto search_result = burst::galloping_lower_bound(range.begin(), range.end(), std::string("bc"));

        BOOST_CHECK(search_result == range.begin() + 1);
        BOOST_CHECK_EQUAL(*search_result, "bc");
    }

    BOOST_AUTO_TEST_CASE(searching_existing_value_results_iterator_to_beginning_of_range_of_equal_values)
    {
        std::vector<int> range{1, 1, 1, 2, 2, 2, 3, 3, 3};

        auto search_result = burst::galloping_lower_bound(range.begin(), range.end(), 3);

        BOOST_CHECK(search_result == range.begin() + 6);
        BOOST_CHECK_EQUAL(*search_result, 3);
    }

    BOOST_AUTO_TEST_CASE(searching_absent_value_results_iterator_to_beginning_of_range_of_equal_values)
    {
        std::vector<int> range{30, 30, 30, 20, 20, 20, 10, 10, 10};

        auto search_result = burst::galloping_lower_bound(range.begin(), range.end(), 25, std::greater<>{});

        BOOST_CHECK(search_result == range.begin() + 3);
        BOOST_CHECK_EQUAL(*search_result, 20);
    }
BOOST_AUTO_TEST_SUITE_END()
