#include <burst/algorithm/galloping_upper_bound.hpp>

#include <doctest/doctest.h>

#include <functional>
#include <list>
#include <ostream>
#include <string>
#include <vector>

TEST_SUITE("galloping_upper_bound")
{
    TEST_CASE("searching_an_empty_range_results_range_end")
    {
        std::vector<int> empty;

        auto search_result = burst::galloping_upper_bound(empty.begin(), empty.end(), 1);

        CHECK(search_result == empty.end());
    }

    TEST_CASE("searching_a_value_greater_than_all_values_in_a_given_range_results_range_end")
    {
        std::vector<std::size_t> range{1, 2, 3, 4, 5};

        auto search_result = burst::galloping_upper_bound(range.begin(), range.end(), 100500ul);

        CHECK(search_result == range.end());
    }

    TEST_CASE("searching_a_value_less_than_all_values_in_a_given_range_results_range_begin")
    {
        std::list<std::int8_t> range{10, 20, 30, 40};

        auto search_result = burst::galloping_upper_bound(range.begin(), range.end(), 0);

        CHECK(search_result == range.begin());
    }

    TEST_CASE("searching_absent_value_results_iterator_to_first_value_greater_than_desired")
    {
        auto range = {50, 40, 30, 20, 10};

        auto search_result = burst::galloping_upper_bound(range.begin(), range.end(), 33, std::greater<>{});

        CHECK(search_result == range.begin() + 2);
        CHECK(*search_result == 30);
    }

    TEST_CASE("searching_existing_value_results_iterator_to_first_value_greater_than_desired")
    {
        std::vector<std::string> range{"abc", "bc", "c"};

        auto search_result = burst::galloping_upper_bound(range.begin(), range.end(), std::string("bc"));

        CHECK(search_result == range.begin() + 2);
        CHECK(*search_result == "c");
    }

    TEST_CASE("searching_existing_value_results_iterator_past_the_end_of_range_of_equal_values")
    {
        std::vector<int> range{1, 1, 1, 2, 2, 2, 3, 3, 3};

        auto search_result = burst::galloping_upper_bound(range.begin(), range.end(), 2);

        CHECK(search_result == range.begin() + 6);
        CHECK(*search_result == 3);
    }

    TEST_CASE("searching_absent_value_results_iterator_to_beginning_of_range_of_equal_values")
    {
        std::vector<int> range{30, 30, 30, 20, 20, 20, 10, 10, 10};

        auto search_result = burst::galloping_upper_bound(range.begin(), range.end(), 25, std::greater<>{});

        CHECK(search_result == range.begin() + 3);
        CHECK(*search_result == 20);
    }

    TEST_CASE("elements_must_be_partitioned_with_respect_to_searched_element")
    {
        auto range = {3, 2, 1, 4, 7, 6, 5};
        //                        |
        //                        |  |
        //                        |  |  |
        //                     |  |  |  |
        //            |        |  |  |  |
        //            |  |     |  |  |  |
        //            |  |  |  |  |  |  |
        //                        ^

        auto search_result = burst::galloping_upper_bound(range.begin(), range.end(), 4);
        CHECK(search_result == range.begin() + 4);
        CHECK(*search_result == 7);
    }

    TEST_CASE("works_with_ranges")
    {
        const auto range = {1, 2, 3, 4, 5};

        const auto search_result = burst::galloping_upper_bound(range, 3);

        CHECK(search_result == range.begin() + 3);
        CHECK(*search_result == 4);
    }
}
