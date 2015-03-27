#include <functional>
#include <iterator>
#include <list>
#include <vector>

#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/range/merge.hpp>

BOOST_AUTO_TEST_SUITE(merge)
    BOOST_AUTO_TEST_CASE(merging_empty_ranges_results_empty_range)
    {
        std::vector<int> first;
        std::vector<int> second;
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto merged_range = burst::merge(ranges);
        BOOST_CHECK(merged_range.empty());
    }

    BOOST_AUTO_TEST_CASE(merging_one_range_results_the_same_range)
    {
        int array[] = {1, 2, 3, 4};
        auto ranges = {boost::make_iterator_range(array)};

        auto merged_range = burst::merge(ranges);
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(array), std::end(array),
            std::begin(merged_range), std::end(merged_range)
        );
    }

    BOOST_AUTO_TEST_CASE(merging_saw_toothed_sorted_ranges_results_sorted_range_containing_all_elements_from_initial_ranges)
    {
        std::list<char> first{'h', 'f', 'd', 'b'};
        std::list<char> second{'g', 'e', 'c', 'a'};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto merged_range = burst::merge(ranges, std::greater<char>());

        std::vector<char> expected_collection{'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a'};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(merged_range), std::end(merged_range),
            std::begin(expected_collection), std::end(expected_collection)
        );
    }

    BOOST_AUTO_TEST_CASE(merging_several_disjoint_sorted_ranges_results_sorted_range_composed_of_joined_initial_ranges)
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

        auto merged_range = burst::merge(ranges, std::less<int>());

        auto expected_collection = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(merged_range), std::end(merged_range),
            std::begin(expected_collection), std::end(expected_collection)
        );
    }

    BOOST_AUTO_TEST_CASE(modifying_merged_mutable_ranges_is_allowed)
    {
        std::vector<int> first{100, 50};
        std::vector<int> second{70, 30};

        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto merged_range = burst::merge(ranges, std::greater<int>());
        boost::for_each(merged_range, [] (int & x) { x /= 10; });

        BOOST_CHECK_EQUAL(first[0], 10);
        BOOST_CHECK_EQUAL(first[1], 5);
        BOOST_CHECK_EQUAL(second[0], 7);
        BOOST_CHECK_EQUAL(second[1], 3);
    }

    BOOST_AUTO_TEST_CASE(merge_function_accepts_inplace_initializer_list)
    {
        std::vector<int> even{2, 4, 6};
        std::vector<int>  odd{1, 3, 5};

        auto merged_range = burst::merge
        ({
            boost::make_iterator_range(even),
            boost::make_iterator_range(odd)
        });

        std::vector<int> all{1, 2, 3, 4, 5, 6};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(all), std::end(all),
            std::begin(merged_range), std::end(merged_range)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
