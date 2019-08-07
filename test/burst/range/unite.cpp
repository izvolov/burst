#include <burst/container/make_list.hpp>
#include <burst/range/make_range_vector.hpp>
#include <burst/range/unite.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>

#include <functional>
#include <string>
#include <vector>

TEST_SUITE("unite")
{
    TEST_CASE("uniting_empty_ranges_results_empty_range")
    {
        std::vector<int> first;
        std::vector<int> second;
        auto ranges = burst::make_range_vector(first, second);

        auto range_union = burst::unite(ranges);

        CHECK(range_union.empty());
    }

    TEST_CASE("uniting_equal_ranges_results_range_equal_to_both_of_them")
    {
        auto first = {4, 5, 6, 7};
        auto second = first;
        auto ranges = burst::make_range_vector(first, second);

        auto range_union = burst::unite(ranges);

        CHECK(range_union == first);
    }

    TEST_CASE("uniting_one_range_results_the_same_range")
    {
        auto only = boost::irange(1, 5);
        auto ranges = burst::make_range_vector(only);
        auto range_union = burst::unite(ranges);

        CHECK(range_union == boost::irange(1, 5));
    }

    TEST_CASE("uniting_nested_ranges_results_longest_of_them")
    {
        std::string  long_range("abcdef");
        std::string short_range("cde");
        auto ranges = burst::make_range_vector(long_range, short_range);

        auto range_union = burst::unite(ranges);

        CHECK(range_union == long_range);
    }

    TEST_CASE("uniting_saw_toothed_ranges_results_range_containing_all_their_elements")
    {
        auto first = burst::make_list({'h', 'f', 'd', 'b'});
        auto second = burst::make_list({'g', 'e', 'c', 'a'});
        auto ranges = burst::make_range_vector(first, second);

        auto range_union = burst::unite(ranges, std::greater<>{});

        CHECK(range_union == std::string("hgfedcba"));
    }

    TEST_CASE("element_of_union_of_overlapping_ranges_do_not_repeat")
    {
        auto  first = {3, 2, 1};
        auto second = {4, 3, 2};
        auto  third = {5, 3, 1};
        auto ranges = burst::make_range_vector(first, second, third);

        auto range_union = burst::unite(ranges, std::greater<>{});

        auto expected_collection = {5, 4, 3, 2, 1};
        CHECK(range_union == expected_collection);
    }

    TEST_CASE("repeating_elements_taken_max_count_among_initial_ranges_times")
    {
        auto  first = {1, 1, 1, 2, 2, 3};
        auto second = {1, 1, 2};
        auto  third = {1};
        auto ranges = burst::make_range_vector(first, second, third);

        auto range_union = burst::unite(ranges);

        auto expected_collection = {1, 1, 1, 2, 2, 3};
        CHECK(range_union == expected_collection);
    }
}
