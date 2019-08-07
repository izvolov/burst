#include <burst/algorithm/select_min.hpp>
#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_vector.hpp>

#include <doctest/doctest.h>

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

TEST_SUITE("select_min")
{
    TEST_CASE("returns_iterator_past_the_last_min_element")
    {
        auto items = burst::make_vector({0, 0, 1, 2, 3});
        //                               ^  ^

        const auto result = burst::select_min(items.begin(), items.end());

        CHECK(result == items.begin() + 2);
    }

    TEST_CASE("all_items_before_result_are_minimal_in_the_range")
    {
        auto items = burst::make_vector({2, 1, 3, 1, 4, 1, 5});
        //                                  ^     ^     ^
        const auto min = *std::min_element(items.begin(), items.end());

        const auto result = burst::select_min(items.begin(), items.end());

        CHECK(std::all_of(items.begin(), result,
            [& min] (const auto & e) {return e == min;}));
    }

    TEST_CASE("searching_empty_range_results_iterator_to_begin")
    {
        auto empty = std::vector<int>{};

        const auto result = burst::select_min(empty.begin(), empty.end());

        CHECK(result == empty.begin());
    }

    TEST_CASE("searching_range_with_equal_elements_returns_iterator_to_end")
    {
        auto single = std::string("aaaaaaa");

        const auto result = burst::select_min(single.begin(), single.end());

        CHECK(result == single.end());
    }

    TEST_CASE("works_with_ranges")
    {
        auto range = burst::make_vector({5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 1});
        //                                                       ^  ^  ^

        const auto result = burst::select_min(range);

        CHECK(result == range.begin() + 3);
        CHECK(range.front() == 1);
    }

    TEST_CASE("accepts_custom_predicate")
    {
        auto items = burst::make_vector({-1, 1, -2, 2, -3, 100500});
        //                                                 ^

        const auto result = burst::select_min(items, std::greater<>{});

        CHECK(result == items.begin() + 1);
        CHECK(items.front() == 100500);
    }

    TEST_CASE("works_with_forward_iterators")
    {
        auto forward_range = burst::make_forward_list({-1, 0, 1, -2, 0, -2});
        //                                                       ^      ^

        const auto result = burst::select_min(forward_range);

        CHECK(result == std::next(forward_range.begin(), 2));
        CHECK(forward_range.front() == -2);
    }
}
