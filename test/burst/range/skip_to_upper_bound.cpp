#include <burst/container/make_vector.hpp>
#include <burst/range/skip_to_upper_bound.hpp>

#include <doctest/doctest.h>

#include <boost/range/iterator_range.hpp>

#include <functional>
#include <vector>

TEST_SUITE("skip_to_upper_bound")
{
    TEST_CASE("skipping_by_value_in_empty_range_results_empty_range")
    {
        std::vector<int> empty_container;
        auto empty_range = boost::make_iterator_range(empty_container);

        burst::skip_to_upper_bound(empty_range, 5);

        CHECK(empty_range.empty());
    }

    TEST_CASE("skipping_by_existing_value_makes_range_begin_after_last_occurence_of_the_value")
    {
        auto values = burst::make_vector({1, 5, 5, 7});
        auto sorted_range = boost::make_iterator_range(values);

        burst::skip_to_upper_bound(sorted_range, 5);

        CHECK(sorted_range == boost::make_iterator_range(values).advance_begin(3));
    }

    TEST_CASE("skipping_by_value_greater_than_all_values_in_range_makes_range_empty")
    {
        auto values = {1, 2, 3};
        auto sorted_range = boost::make_iterator_range(values);

        burst::skip_to_upper_bound(sorted_range, 100500);

        CHECK(sorted_range.empty());
    }

    TEST_CASE("skipping_by_value_less_than_all_elements_in_range_does_not_modify_range")
    {
        auto values = {3, 2, 1};
        auto sorted_range = boost::make_iterator_range(values);
        auto sorted_range_copy = sorted_range;

        burst::skip_to_upper_bound(sorted_range, 100500, std::greater<>{});

        CHECK(sorted_range == sorted_range_copy);
    }

    TEST_CASE("skipping_by_value_absent_in_sorted_range_makes_range_begin_at_first_value_greater_than_desired")
    {
        std::vector<double> values{3.14, 2.71, 1.6};
        auto sorted_range = boost::make_iterator_range(values);

        burst::skip_to_upper_bound(sorted_range, 2.5, std::greater<>{});

        CHECK(sorted_range == boost::make_iterator_range(values).advance_begin(2));
    }
}
