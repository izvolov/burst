#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/intersect.hpp>
#include <burst/range/make_range_vector.hpp>

#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <functional>
#include <iterator>
#include <string>
#include <vector>

BOOST_AUTO_TEST_SUITE(intersect)
    BOOST_AUTO_TEST_CASE(intersecting_empty_ranges_results_empty_range)
    {
        std::vector<int> first;
        std::vector<int> second;
        auto ranges = burst::make_range_vector(first, second);

        auto intersected_range = burst::intersect(ranges);

        BOOST_CHECK(intersected_range.empty());
    }

    BOOST_AUTO_TEST_CASE(intersecting_one_range_results_the_same_range)
    {
        auto only = boost::irange(1, 5);
        auto ranges = burst::make_range_vector(only);
        auto intersected_range = burst::intersect(ranges);

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(boost::irange(1, 5)), std::end(boost::irange(1, 5)),
            std::begin(intersected_range), std::end(intersected_range)
        );
    }

    BOOST_AUTO_TEST_CASE(intersecting_equal_ranges_results_range_equal_to_both_of_them)
    {
        auto first = {4, 5, 6, 7};
        auto second = first;
        auto ranges = burst::make_range_vector(first, second);

        auto intersected_range = burst::intersect(ranges);

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(intersected_range), std::end(intersected_range),
            std::begin(first), std::end(first)
        );
    }

    BOOST_AUTO_TEST_CASE(intersecting_nested_ranges_results_shortest_of_them)
    {
        std::string  long_range("abcdef");
        std::string short_range("cde");
        auto ranges = burst::make_range_vector(long_range, short_range);

        auto intersected_range = burst::intersect(ranges);

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(intersected_range), std::end(intersected_range),
            std::begin(short_range), std::end(short_range)
        );
    }

    BOOST_AUTO_TEST_CASE(intersecting_saw_toothed_ranges_results_empty_range)
    {
        auto  first = burst::make_list({'h',      'f',      'd',      'b'     });
        auto second = burst::make_list({     'g',      'e',      'c',      'a'});
        auto ranges = burst::make_range_vector(first, second);

        auto intersected_range = burst::intersect(ranges, std::greater<>());

        BOOST_CHECK(intersected_range.empty());
    }

    BOOST_AUTO_TEST_CASE(intersecting_two_overlaying_ranges_results_overlayed_part)
    {
        auto  first = {   3, 2, 1};
        auto second = {4, 3, 2   };
        //                ^  ^
        auto ranges = burst::make_range_vector(first, second);

        auto intersected_range = burst::intersect(ranges, std::greater<>());

        auto expected_collection = {3, 2};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(intersected_range), std::end(intersected_range),
            std::begin(expected_collection), std::end(expected_collection)
        );
    }

    BOOST_AUTO_TEST_CASE(intersecting_pairwise_overlaying_but_mutual_disjoint_ranges_results_empty_range)
    {
        auto  first = {1, 2      };
        auto second = {   2, 3   };
        auto  third = {      3, 4};
        auto ranges = burst::make_range_vector(first, second, third);

        auto intersected_range = burst::intersect(ranges);

        BOOST_CHECK(intersected_range.empty());
    }

    BOOST_AUTO_TEST_CASE(intersecting_several_consecutive_sorted_ranges_results_empty_range)
    {
        auto  first = {1, 2, 3                  };
        auto second = {         4, 5, 6         };
        auto  third = {                  7, 8, 9};
        auto ranges = burst::make_range_vector(first, second, third);

        auto intersected_range = burst::intersect(ranges, std::less<>());

        BOOST_CHECK(intersected_range.empty());
    }

    BOOST_AUTO_TEST_CASE(repeating_elements_do_not_produce_excess_matches)
    {
        auto  first = {1, 1, 1};
        auto second = {1, 1, 1};
        auto  third = {1, 1, 1};
        auto ranges = burst::make_range_vector(first, second, third);

        auto intersected_range = burst::intersect(ranges, std::less<>());

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(intersected_range), std::end(intersected_range),
            std::begin(first), std::end(first)
        );
    }

    BOOST_AUTO_TEST_CASE(no_element_considered_twice)
    {
        auto  first = {0, 0, 1};
        auto second = {0, 1, 1};
        auto ranges = burst::make_range_vector(first, second);

        auto intersected_range = burst::intersect(ranges);

        auto expected_collection = {0, 1};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(intersected_range), std::end(intersected_range),
            std::begin(expected_collection), std::end(expected_collection)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
