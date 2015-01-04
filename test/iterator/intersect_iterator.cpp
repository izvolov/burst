#include <vector>
#include <list>

#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/iterator/intersect_iterator.hpp>
#include <burst/range/intersect.hpp>

BOOST_AUTO_TEST_SUITE(intersect_iterator)
    BOOST_AUTO_TEST_CASE(intersecting_empty_ranges_results_empty_range)
    {
        std::vector<int> first;
        std::vector<int> second;
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto intersected_range = burst::intersect(ranges);

        BOOST_CHECK(intersected_range.empty());
    }

    BOOST_AUTO_TEST_CASE(intersecting_one_range_results_the_same_range)
    {
        int array[] = {1, 2, 3, 4};
        auto ranges = {boost::make_iterator_range(array)};

        auto intersected_range = burst::intersect(ranges);

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(array), boost::end(array),
            boost::begin(intersected_range), boost::end(intersected_range)
        );
    }

    BOOST_AUTO_TEST_CASE(intersecting_equal_ranges_results_range_equal_to_both_of_them)
    {
        auto first = {4, 5, 6, 7};
        auto second = first;
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto intersected_range = burst::intersect(ranges);

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(intersected_range), boost::end(intersected_range),
            boost::begin(first), boost::end(first)
        );
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(intersected_range), boost::end(intersected_range),
            boost::begin(second), boost::end(second)
        );
    }

    BOOST_AUTO_TEST_CASE(intersecting_nested_ranges_results_shortest_of_them)
    {
        std::string  long_range("abcdef");
        std::string short_range("cde");
        auto ranges = {boost::make_iterator_range(long_range), boost::make_iterator_range(short_range)};

        auto intersected_range = burst::intersect(ranges);

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(intersected_range), boost::end(intersected_range),
            boost::begin(short_range), boost::end(short_range)
        );
    }

    BOOST_AUTO_TEST_CASE(intersecting_saw_toothed_ranges_results_empty_range)
    {
        std::list<char> first{'h', 'f', 'd', 'b'};
        std::list<char> second{'g', 'e', 'c', 'a'};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto intersected_range = burst::intersect(ranges, std::greater<char>());

        BOOST_CHECK(intersected_range.empty());
    }

    BOOST_AUTO_TEST_CASE(intersecting_two_overlaying_ranges_results_overlayed_part)
    {
        auto  first = {3, 2, 1};
        auto second = {4, 3, 2};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto intersected_range = burst::intersect(ranges, std::greater<int>());

        std::vector<int> expected_collection{3, 2};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(intersected_range), boost::end(intersected_range),
            boost::begin(expected_collection), boost::end(expected_collection)
        );
    }

    BOOST_AUTO_TEST_CASE(intersecting_pairwise_overlaying_but_mutual_disjoint_ranges_results_empty_range)
    {
        auto  first = {1, 2};
        auto second = {2, 3};
        auto  third = {3, 4};
        auto ranges =
        {
            boost::make_iterator_range(first),
            boost::make_iterator_range(second),
            boost::make_iterator_range(third)
        };

        auto intersected_range = burst::intersect(ranges);

        BOOST_CHECK(intersected_range.empty());
    }

    BOOST_AUTO_TEST_CASE(intersecting_several_consecutive_sorted_ranges_results_empty_range)
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

        auto intersected_range = burst::intersect(ranges, std::less<int>());

        BOOST_CHECK(intersected_range.empty());
    }

    BOOST_AUTO_TEST_CASE(repeating_elements_do_not_produce_excess_matches)
    {
        auto  first = {1, 1, 1};
        auto second = {1, 1, 1};
        auto  third = {1, 1, 1};
        auto ranges =
        {
            boost::make_iterator_range(first),
            boost::make_iterator_range(second),
            boost::make_iterator_range(third)
        };

        auto intersected_range = burst::intersect(ranges, std::less<int>());

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(intersected_range), boost::end(intersected_range),
            boost::begin(first), boost::end(first)
        );
    }

    BOOST_AUTO_TEST_CASE(intersect_iterator_end_is_created_using_special_tag)
    {
        auto  first = {1, 2, 3};
        auto second = {0, 2, 4};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto intersected_begin = burst::make_intersect_iterator(ranges);
        auto   intersected_end = burst::make_intersect_iterator(ranges, burst::iterator::end_tag);

        auto expected_collection = {2};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            intersected_begin, intersected_end,
            boost::begin(expected_collection), boost::end(expected_collection)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
