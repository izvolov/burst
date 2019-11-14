#include <burst/container/k_ary_search_set.hpp>
#include <burst/container/make_vector.hpp>

#include <doctest/doctest.h>

#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

namespace burst
{
    template <typename Value, typename Compare, typename Collection>
    bool operator == (const k_ary_search_set<Value, Compare> & set, const Collection & c)
    {
        return
            std::mismatch
            (
                set.begin(), set.end(),
                c.begin(), c.end()
            ) == std::make_pair(set.end(), c.end());
    }
} // namespace burst

TEST_SUITE("k_ary_search")
{
    TEST_CASE("k_ary_search_set_initialized_with_default_constructor_is_empty")
    {
        burst::k_ary_search_set<std::string> set;
        CHECK(set.empty());
    }

    TEST_CASE("size_of_k_ary_search_set_initialized_with_default_constructor_is_zero")
    {
        burst::k_ary_search_set<std::string> set;
        CHECK(set.size() == 0);
    }

    TEST_CASE("begin_of_k_ary_search_set_initialized_with_default_constructor_is_equal_to_end")
    {
        burst::k_ary_search_set<std::string> set;
        CHECK(set.begin() == set.end());
    }

    TEST_CASE("nothing_to_find_in_k_ary_search_set_initialized_with_default_constructor")
    {
        burst::k_ary_search_set<std::string> set;
        CHECK(set.find("qwerty") == set.end());
    }

    TEST_CASE("k_ary_search_set_is_empty_when_initialized_from_empty_range")
    {
        std::vector<int> nothing;

        burst::k_ary_search_set<int> set(nothing.begin(), nothing.end());

        CHECK(set.empty());
    }

    TEST_CASE("size_of_k_ary_search_set_initialized_from_empty_range_is_zero")
    {
        std::vector<int> nothing;

        burst::k_ary_search_set<int> set(nothing.begin(), nothing.end());

        CHECK(set.size() == 0);
    }

    TEST_CASE("k_ary_search_set_size_is_equal_to_size_of_range_it_is_initialized_from")
    {
        auto something = burst::make_vector({1, 2, 3, 4});

        burst::k_ary_search_set<int> set(burst::container::unique_ordered_tag, something.begin(), something.end());

        CHECK(set.size() == something.size());
    }

    TEST_CASE("k_ary_search_set_is_initialized_properly_when_it_is_perfect_ternary_tree")
    {
        auto numbers = burst::make_vector({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});

        burst::k_ary_search_set<int> set(numbers.begin(), numbers.end(), 3);

        auto expected = {7, 10, 2, 5, 8, 9, 11, 12, 0, 1, 3, 4, 6};
        CHECK(set == expected);
    }

    TEST_CASE("k_ary_search_set_is_initialized_properly_when_it_is_complete_ternary_tree")
    {
        burst::k_ary_search_set<int> set(burst::container::unique_ordered_tag, {0, 1, 2, 3, 4, 5, 6, 7}, 3);

        auto expected = {2, 5, 0, 1, 3, 4, 6, 7};
        CHECK(set == expected);
    }

    TEST_CASE("k_ary_search_set_is_initialized_properly_when_arity_is_greater_than_its_size")
    {
        auto numbers = burst::make_vector({0, 1, 2, 3, 4, 5, 6, 7});

        burst::k_ary_search_set<int> set(numbers.begin(), numbers.end(), 129);

        CHECK(set == numbers);
    }

    TEST_CASE("nothing_is_found_when_searching_empty_range")
    {
        std::vector<int> nothing;

        burst::k_ary_search_set<int> set(nothing.begin(), nothing.end());

        CHECK(set.find(26) == set.end());
    }

    TEST_CASE("searching_absent_value_results_end_iterator")
    {
        burst::k_ary_search_set<int> set({3, 4, 6, 1, 7, 8, 2}, 2);

        CHECK(set.find(26) == set.end());
    }

    TEST_CASE("searching_existing_value_results_non_end_iterator")
    {
        auto numbers = burst::make_vector({3, 4, 6, 1, 7, 8, 2});

        burst::k_ary_search_set<int> set(numbers.begin(), numbers.end(), 3);

        CHECK(set.find(8) != set.end());
    }

    TEST_CASE("searching_existing_value_results_iterator_pointing_to_that_value")
    {
        auto numbers = burst::make_vector({3, 4, 6, 1, 7, 8, 2});

        burst::k_ary_search_set<int, std::greater<>> set(numbers.begin(), numbers.end(), 4);

        CHECK(*set.find(3) == 3);
    }

    TEST_CASE("no_logarithm_rounding_error")
    {
        std::vector<int> numbers(1000);
        std::iota(numbers.rbegin(), numbers.rend(), 0);

        burst::k_ary_search_set<int, std::greater<>> set(numbers.begin(), numbers.end(), 10);

        CHECK(*set.find(50) == 50);
    }

    TEST_CASE("const_find_method_exists")
    {
        const burst::k_ary_search_set<int> set;
        CHECK(set.find(8) == set.end());
    }

    TEST_CASE("mutable_find_method_exists")
    {
        burst::k_ary_search_set<int> set;
        burst::k_ary_search_set<int> & set_ref = set;
        CHECK(set_ref.find(8) == set_ref.end());
    }
}
