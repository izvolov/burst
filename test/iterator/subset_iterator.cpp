#include <burst/iterator/subset_iterator.hpp>

#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <iterator>
#include <list>
#include <vector>

BOOST_AUTO_TEST_SUITE(subset_iterator)
    BOOST_AUTO_TEST_CASE(empty_set_has_no_subsets)
    {
        std::vector<int> set;

        auto subsets_begin = burst::make_subset_iterator(boost::make_iterator_range(set));
        auto   subsets_end = burst::make_subset_iterator(burst::iterator::end_tag, subsets_begin);

        BOOST_CHECK(subsets_begin == subsets_end);
    }

    BOOST_AUTO_TEST_CASE(singleton_set_has_one_subset_equal_to_whole_set)
    {
        std::list<int> set{3};

        auto      subset = burst::make_subset_iterator(boost::make_iterator_range(set));
        auto subsets_end = burst::make_subset_iterator(burst::iterator::end_tag, subset);

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
        auto subsets_begin = burst::make_subset_iterator(boost::irange(1, 4));
        auto   subsets_end = burst::make_subset_iterator(burst::iterator::end_tag, subsets_begin);

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

        auto subsets_begin = burst::make_subset_iterator(boost::make_iterator_range(sequence));
        auto   subsets_end = burst::make_subset_iterator(burst::iterator::end_tag, subsets_begin);

        BOOST_CHECK_EQUAL(std::distance(subsets_begin, subsets_end), 1);
        BOOST_CHECK_EQUAL(subsets_begin->size(), 1);
    }
BOOST_AUTO_TEST_SUITE_END()
