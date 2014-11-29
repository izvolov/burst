#include <vector>
#include <list>

#include <boost/bind.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/test/unit_test.hpp>

#include <thrust/iterator/merge_iterator.hpp>
#include <thrust/range/merge.hpp>

BOOST_AUTO_TEST_SUITE(merge_iterator)
    BOOST_AUTO_TEST_CASE(merging_empty_ranges_results_empty_range)
    {
        std::vector<int> first;
        std::vector<int> second;
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto merged_range = thrust::merge(ranges);
        BOOST_CHECK(merged_range.empty());
    }

    BOOST_AUTO_TEST_CASE(merging_one_range_results_the_same_range)
    {
        int array[] = {1, 2, 3, 4};
        auto ranges = {boost::make_iterator_range(array)};

        auto merged_range = thrust::merge(ranges);
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(array), boost::end(array),
            boost::begin(merged_range), boost::end(merged_range)
        );
    }

    BOOST_AUTO_TEST_CASE(merging_two_intersecting_sorted_ranges_results_correct_sorted_range)
    {
        std::list<char> first{'h', 'f', 'd', 'b'};
        std::list<char> second{'g', 'e', 'c', 'a'};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto merged_range = thrust::merge(ranges, std::greater<char>());

        std::vector<char> expected_collection{'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a'};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(merged_range), boost::end(merged_range),
            boost::begin(expected_collection), boost::end(expected_collection)
        );
    }

    BOOST_AUTO_TEST_CASE(merging_several_disjoint_sorted_ranges_results_correct_sorted_range)
    {
        auto  first = {1, 2, 3};
        auto second = {4, 5, 6};
        auto  third = {7, 8, 9};
        auto ranges =
        {
            boost::make_iterator_range(first),
            boost::make_iterator_range(second),
            boost::make_iterator_range(third)
        };

        auto merged_range = thrust::merge(ranges, std::less<int>());

        auto expected_collection = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(merged_range), boost::end(merged_range),
            boost::begin(expected_collection), boost::end(expected_collection)
        );
    }

    BOOST_AUTO_TEST_CASE(modifying_merged_mutable_ranges_is_allowed)
    {
        std::vector<int> first{100, 50};
        std::vector<int> second{70, 30};

        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto merged_range = thrust::merge(ranges, std::greater<int>());
        boost::for_each(merged_range, [] (int & x) { x /= 10; });

        BOOST_CHECK_EQUAL(first[0], 10);
        BOOST_CHECK_EQUAL(first[1], 5);
        BOOST_CHECK_EQUAL(second[0], 7);
        BOOST_CHECK_EQUAL(second[1], 3);
    }

    BOOST_AUTO_TEST_CASE(merge_iterator_end_is_created_using_special_tag)
    {
        auto  first = {500, 100};
        auto second = {600, 200};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto merged_begin = thrust::make_merge_iterator(ranges, std::greater<int>());
        auto   merged_end = thrust::make_merge_iterator(ranges, std::greater<int>(), thrust::iterator::end_tag);

        auto expected_collection = {600, 500, 200, 100};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            merged_begin, merged_end,
            boost::begin(expected_collection), boost::end(expected_collection)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
