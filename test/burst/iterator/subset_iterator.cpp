#include <burst/iterator/subset_iterator.hpp>
#include <utility/io/vector.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>

#include <iterator>
#include <list>
#include <vector>

TEST_SUITE("subset_iterator")
{
    TEST_CASE("subset_iterator_end_is_created_using_special_tag")
    {
        const auto set = {'a', 'b', 'c'};

        auto subsets_begin = burst::make_subset_iterator(set);
        auto subsets_end = burst::make_subset_iterator(burst::iterator::end_tag, subsets_begin);

        const auto expected_subsets =
            std::vector<std::vector<char>>
            {
                {'a'}, {'b'}, {'c'},
                {'a', 'b'}, {'a', 'c'}, {'b', 'c'},
                {'a', 'b', 'c'}
            };
        CHECK(boost::make_iterator_range(subsets_begin, subsets_end) == expected_subsets);
    }

    TEST_CASE("empty_set_has_no_subsets")
    {
        std::vector<int> set;

        auto subsets_begin = burst::make_subset_iterator(set);
        auto   subsets_end = burst::make_subset_iterator(burst::iterator::end_tag, subsets_begin);

        CHECK(subsets_begin == subsets_end);
    }

    TEST_CASE("singleton_set_has_one_subset_equal_to_whole_set")
    {
        std::list<int> set{3};

        auto      subset = burst::make_subset_iterator(set);
        auto subsets_end = burst::make_subset_iterator(burst::iterator::end_tag, subset);

        std::vector<int> expected_subset{3};
        CHECK(*subset == expected_subset);
        CHECK(std::distance(subset, subsets_end) == 1);
    }

    TEST_CASE("set_of_three_unique_elements_has_seven_non_empty_subsets")
    {
        auto subsets_begin = burst::make_subset_iterator(boost::irange(1, 4));
        auto   subsets_end = burst::make_subset_iterator(burst::iterator::end_tag, subsets_begin);

        CHECK(std::distance(subsets_begin, subsets_end) == 7);

        std::vector<std::vector<int>> expected_subsets =
        {
            {1}, {2}, {3},
            {1, 2}, {1, 3}, {2, 3},
            {1, 2, 3}
        };
        CHECK(boost::make_iterator_range(subsets_begin, subsets_end) == expected_subsets);
    }

    TEST_CASE("sequence_of_several_identical_elements_has_only_one_singleton_subset")
    {
        auto sequence = {1, 1, 1, 1, 1};

        auto subsets_begin = burst::make_subset_iterator(sequence);
        auto   subsets_end = burst::make_subset_iterator(burst::iterator::end_tag, subsets_begin);

        CHECK(std::distance(subsets_begin, subsets_end) == 1);
        CHECK(subsets_begin->size() == 1);
    }
}
