#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/make_range_vector.hpp>
#include <burst/range/symmetric_difference.hpp>

#include <boost/range/irange.hpp>
#include <boost/test/unit_test.hpp>

#include <functional>
#include <vector>

BOOST_AUTO_TEST_SUITE(symmetric_difference)
    BOOST_AUTO_TEST_CASE(consists_of_elements_occured_in_odd_number_of_input_ranges)
    {
        const auto natural = burst::make_vector({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
        const auto     odd = burst::make_vector({1,    3,    5,    7,    9,     11    });
        const auto   prime = burst::make_vector({   2, 3,    5,    7,           11    });
        //                                             ^  ^  ^  ^  ^  ^     ^   ^   ^
        auto ranges = burst::make_range_vector(natural, odd, prime);

        const auto symmetric_difference = burst::symmetric_difference(ranges);

        const auto expected = {3, 4, 5, 6, 7, 8, 10, 11, 12};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(symmetric_difference), std::end(symmetric_difference),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(symmetric_difference_of_empty_ranges_results_empty_range)
    {
        const auto first = std::vector<int>{};
        const auto second = std::vector<int>{};
        auto ranges = burst::make_range_vector(first, second);

        const auto symmetric_difference = burst::symmetric_difference(ranges);

        BOOST_CHECK(symmetric_difference.empty());
    }

    BOOST_AUTO_TEST_CASE(symmetric_difference_of_one_range_results_the_same_range)
    {
        const auto only = boost::irange(1, 5);
        auto ranges = burst::make_range_vector(only);

        const auto symmetric_difference = burst::symmetric_difference(ranges);

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(boost::irange(1, 5)), std::end(boost::irange(1, 5)),
            std::begin(symmetric_difference), std::end(symmetric_difference)
        );
    }

    BOOST_AUTO_TEST_CASE(symmetric_difference_of_odd_number_of_equal_ranges_equals_to_any_of_those_ranges)
    {
        const auto first = {4, 4, 5, 6, 6, 7};
        const auto second = first;
        const auto third = first;

        auto ranges = burst::make_range_vector(first, second, third);
        const auto symmetric_difference = burst::symmetric_difference(ranges);

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(symmetric_difference), std::end(symmetric_difference),
            std::begin(first), std::end(first)
        );
    }

    BOOST_AUTO_TEST_CASE(symmetric_difference_of_even_number_of_equal_ranges_is_empty)
    {
        const auto first = {4, 4, 5, 6, 6, 7};
        const auto second = first;
        const auto third = first;
        const auto fourth = first;

        auto ranges = burst::make_range_vector(first, second, third, fourth);
        const auto symmetric_difference = burst::symmetric_difference(ranges);

        BOOST_CHECK(symmetric_difference.empty());
    }

    BOOST_AUTO_TEST_CASE(symmetric_difference_of_disjoint_ranges_results_their_union)
    {
        const auto  first = burst::make_list({'a',           'd',           'g'     });
        const auto second = burst::make_list({     'b',           'e',           'h'});
        const auto  third = burst::make_list({          'c',           'f',         });
        auto ranges = burst::make_range_vector(first, second, third);

        const auto symmetric_difference = burst::symmetric_difference(ranges);

        const auto expected = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(symmetric_difference), std::end(symmetric_difference),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(symmetric_difference_supports_custom_ordering)
    {
        const auto  first = {   3, 3, 2, 1};
        const auto second = {4, 3,    2   };
        //                   ^     ^     ^
        auto ranges = burst::make_range_vector(first, second);

        const auto symmetric_difference = burst::symmetric_difference(ranges, std::greater<>{});

        const auto expected = {4, 3, 1};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(symmetric_difference), std::end(symmetric_difference),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(symmetric_difference_of_several_consecutive_sorted_ranges_results_their_union)
    {
        const auto  first = {1, 2, 3                  };
        const auto second = {         4, 5, 6         };
        const auto  third = {                  7, 8, 9};
        //                   ^  ^  ^  ^  ^  ^  ^  ^  ^
        auto ranges = burst::make_range_vector(first, second, third);

        const auto symmetric_difference = burst::symmetric_difference(ranges, std::less<>{});

        const auto expected = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(symmetric_difference), std::end(symmetric_difference),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(repeating_elements_do_not_produce_excess_matches)
    {
        const auto  first = {1, 1, 1};
        const auto second = {1, 1, 1};
        const auto  third = {1, 1, 1};
        //                   ^  ^  ^
        auto ranges = burst::make_range_vector(first, second, third);

        const auto symmetric_difference = burst::symmetric_difference(ranges, std::less<>{});

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(symmetric_difference), std::end(symmetric_difference),
            std::begin(first), std::end(first)
        );
    }

    BOOST_AUTO_TEST_CASE(ranges_are_considered_multisets)
    {
        const auto  first = {0, 0, 1,       2};
        const auto second = {0,    1, 1      };
        const auto  third = {      1, 1, 1, 2};
        //                      ^  ^     ^

        auto ranges = burst::make_range_vector(first, second, third);
        const auto symmetric_difference = burst::symmetric_difference(ranges);

        const auto expected = {0, 1, 1};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(symmetric_difference), std::end(symmetric_difference),
            std::begin(expected), std::end(expected)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
