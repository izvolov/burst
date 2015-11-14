#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <burst/container/k_ary_search_set.hpp>

BOOST_AUTO_TEST_SUITE(k_ary_search)
    BOOST_AUTO_TEST_CASE(k_ary_search_set_initialized_with_default_constructor_is_empty)
    {
        burst::k_ary_search_set<std::string> set;
        BOOST_CHECK(set.empty());
    }

    BOOST_AUTO_TEST_CASE(size_of_k_ary_search_set_initialized_with_default_constructor_is_zero)
    {
        burst::k_ary_search_set<std::string> set;
        BOOST_CHECK_EQUAL(set.size(), 0);
    }

    BOOST_AUTO_TEST_CASE(begin_of_k_ary_search_set_initialized_with_default_constructor_is_equal_to_end)
    {
        burst::k_ary_search_set<std::string> set;
        BOOST_CHECK(set.begin() == set.end());
    }

    BOOST_AUTO_TEST_CASE(nothing_to_find_in_k_ary_search_set_initialized_with_default_constructor)
    {
        burst::k_ary_search_set<std::string> set;
        BOOST_CHECK(set.find("qwerty") == set.end());
    }

    BOOST_AUTO_TEST_CASE(k_ary_search_set_is_empty_when_initialized_from_empty_range)
    {
        std::vector<int> nothing;

        burst::k_ary_search_set<int> set(nothing.begin(), nothing.end());

        BOOST_CHECK(set.empty());
    }

    BOOST_AUTO_TEST_CASE(size_of_k_ary_search_set_initialized_from_empty_range_is_zero)
    {
        std::vector<int> nothing;

        burst::k_ary_search_set<int> set(nothing.begin(), nothing.end());

        BOOST_CHECK_EQUAL(set.size(), 0);
    }

    BOOST_AUTO_TEST_CASE(k_ary_search_set_size_is_equal_to_size_of_range_it_is_initialized_from)
    {
        std::vector<int> something{1, 2, 3, 4};

        burst::k_ary_search_set<int> set(burst::container::unique_ordered_tag, something.begin(), something.end());

        BOOST_CHECK_EQUAL(set.size(), something.size());
    }

    BOOST_AUTO_TEST_CASE(k_ary_search_set_is_initialized_properly_when_it_is_perfect_ternary_tree)
    {
        std::vector<int> numbers{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

        burst::k_ary_search_set<int> set(numbers.begin(), numbers.end(), 3);

        auto expected = {7, 10, 2, 5, 8, 9, 11, 12, 0, 1, 3, 4, 6};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            set.begin(), set.end(),
            expected.begin(), expected.end()
        );
    }

    BOOST_AUTO_TEST_CASE(k_ary_search_set_is_initialized_properly_when_it_is_complete_ternary_tree)
    {
        burst::k_ary_search_set<int> set(burst::container::unique_ordered_tag, {0, 1, 2, 3, 4, 5, 6, 7}, 3);

        auto expected = {2, 5, 0, 1, 3, 4, 6, 7};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            set.begin(), set.end(),
            expected.begin(), expected.end()
        );
    }

    BOOST_AUTO_TEST_CASE(k_ary_search_set_is_initialized_properly_when_arity_is_greater_than_its_size)
    {
        std::vector<int> numbers{0, 1, 2, 3, 4, 5, 6, 7};

        burst::k_ary_search_set<int> set(numbers.begin(), numbers.end(), 129);

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            set.begin(), set.end(),
            numbers.begin(), numbers.end()
        );
    }

    BOOST_AUTO_TEST_CASE(nothing_is_found_when_searching_empty_range)
    {
        std::vector<int> nothing;

        burst::k_ary_search_set<int> set(nothing.begin(), nothing.end());

        BOOST_CHECK(set.find(26) == set.end());
    }

    BOOST_AUTO_TEST_CASE(searching_absent_value_results_end_iterator)
    {
        burst::k_ary_search_set<int> set({3, 4, 6, 1, 7, 8, 2}, 2);

        BOOST_CHECK(set.find(26) == set.end());
    }

    BOOST_AUTO_TEST_CASE(searching_existing_value_results_non_end_iterator)
    {
        std::vector<int> numbers{3, 4, 6, 1, 7, 8, 2};

        burst::k_ary_search_set<int> set(numbers.begin(), numbers.end(), 3);

        BOOST_CHECK(set.find(8) != set.end());
    }

    BOOST_AUTO_TEST_CASE(searching_existing_value_results_iterator_pointing_to_that_value)
    {
        std::vector<int> numbers{3, 4, 6, 1, 7, 8, 2};

        burst::k_ary_search_set<int, std::greater<int>> set(numbers.begin(), numbers.end(), 4);

        BOOST_CHECK_EQUAL(*set.find(3), 3);
    }

    BOOST_AUTO_TEST_CASE(no_logarithm_rounding_error)
    {
        std::vector<int> numbers(1000);
        std::iota(numbers.rbegin(), numbers.rend(), 0);

        burst::k_ary_search_set<int, std::greater<int>> set(numbers.begin(), numbers.end(), 10);

        BOOST_CHECK_EQUAL(*set.find(50), 50);
    }

    BOOST_AUTO_TEST_CASE(const_find_method_exists)
    {
        const burst::k_ary_search_set<int> set;
        BOOST_CHECK(set.find(8) == set.end());
    }

    BOOST_AUTO_TEST_CASE(mutable_find_method_exists)
    {
        burst::k_ary_search_set<int> set;
        burst::k_ary_search_set<int> & set_ref = set;
        BOOST_CHECK(set_ref.find(8) == set_ref.end());
    }
BOOST_AUTO_TEST_SUITE_END()
