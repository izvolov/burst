#include <list>
#include <set>
#include <vector>

#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/iterator/subset_iterator.hpp>
#include <burst/range/subset.hpp>

BOOST_AUTO_TEST_SUITE(subset_iterator)
    BOOST_AUTO_TEST_CASE(empty_set_has_no_subsets)
    {
        std::vector<int> set;

        auto subsets_begin = burst::make_subset_iterator(set);
        auto   subsets_end = burst::make_subset_iterator(set, burst::iterator::end_tag);

        BOOST_CHECK(subsets_begin == subsets_end);
    }

    BOOST_AUTO_TEST_CASE(singleton_set_has_one_subset_equal_to_whole_set)
    {
        std::list<int> set{3};

        auto      subset = burst::make_subset_iterator(set);
        auto subsets_end = burst::make_subset_iterator(set, burst::iterator::end_tag);

        std::vector<int> expected_subset{3};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            subset->begin(), subset->end(),
            expected_subset.begin(), expected_subset.end()
        );
        BOOST_CHECK_EQUAL(std::distance(subset, subsets_end), 1);
    }

    BOOST_AUTO_TEST_CASE(set_of_three_unique_elements_has_seven_non_empty_subsets)
    {
        auto set = {1, 2, 3};

        auto subsets_begin = burst::make_subset_iterator(set);
        auto   subsets_end = burst::make_subset_iterator(set, burst::iterator::end_tag);

        BOOST_CHECK_EQUAL(std::distance(subsets_begin, subsets_end), 7);

        std::vector<std::vector<int>> expected_subsets =
        {
            {1}, {2}, {3},
            {1, 2}, {1, 3}, {2, 3},
            {1, 2, 3}
        };
        BOOST_CHECK(boost::make_iterator_range(subsets_begin, subsets_end) == expected_subsets);
    }

    BOOST_AUTO_TEST_CASE(sequence_of_several_identical_elements_has_only_one_singleton_subset)
    {
        auto sequence = {1, 1, 1, 1, 1};

        auto subsets_begin = burst::make_subset_iterator(sequence);
        auto   subsets_end = burst::make_subset_iterator(sequence, burst::iterator::end_tag);

        BOOST_CHECK_EQUAL(std::distance(subsets_begin, subsets_end), 1);
    }

    BOOST_AUTO_TEST_CASE(repeating_elements_are_not_considered)
    {
        auto sequence = {3, 3, 3, 2, 2, 2, 1, 1, 1};
        std::set<int, std::greater<int>> set(sequence.begin(), sequence.end());

        auto sequence_subsets = burst::subsets(sequence, std::greater<int>());
        auto set_subsets = burst::subsets(set, std::greater<int>());

        BOOST_CHECK_EQUAL(sequence_subsets, set_subsets);
    }
BOOST_AUTO_TEST_SUITE_END()
