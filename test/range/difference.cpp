#include <functional>
#include <vector>

#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm/set_algorithm.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/container/make_list.hpp>
#include <burst/container/make_set.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/difference.hpp>

BOOST_AUTO_TEST_SUITE(difference)
    BOOST_AUTO_TEST_CASE(difference_of_two_ranges_is_set_of_elements_in_first_range_but_not_in_second_range)
    {
        const auto natural = burst::make_vector({1, 2, 3, 4, 5, 6});
        const auto odd = burst::make_vector({1, 3, 5});

        const auto difference =
            burst::difference
            (
                boost::make_iterator_range(natural),
                boost::make_iterator_range(odd)
            );

        const auto even = burst::make_vector({2, 4, 6});
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            difference.begin(), difference.end(),
            even.begin(), even.end()
        );
    }

    BOOST_AUTO_TEST_CASE(accepts_multisets)
    {
        const auto minuend = burst::make_vector({1, 1, 1, 2, 2, 3});
        const auto subtrahend = burst::make_vector({1, 1, 2});

        const auto difference =
            burst::difference
            (
                boost::make_iterator_range(minuend),
                boost::make_iterator_range(subtrahend)
            );

        const auto expected = burst::make_vector({1, 2, 3});
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            difference.begin(), difference.end(),
            expected.begin(), expected.end()
        );
    }

    BOOST_AUTO_TEST_CASE(set_minus_itself_is_empty_set)
    {
        const auto set = burst::make_vector({'a', 'b', 'c'});

        const auto difference =
            burst::difference
            (
                boost::make_iterator_range(set),
                boost::make_iterator_range(set)
            );

        BOOST_CHECK(difference.empty());
    }

    BOOST_AUTO_TEST_CASE(a_set_minus_empty_set_is_initial_set)
    {
        const auto set = burst::make_vector({3, 2, 1});
        const auto empty_set = std::vector<int>{};

        const auto difference =
            burst::difference
            (
                boost::make_iterator_range(set),
                boost::make_iterator_range(empty_set),
                std::greater<>()
            );

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            difference.begin(), difference.end(),
            set.begin(), set.end()
        );
    }

    BOOST_AUTO_TEST_CASE(empty_set_minus_any_set_is_empty_set)
    {
        const auto empty_set = std::vector<int>{};
        const auto set = burst::make_set({1, 2, 3});

        const auto difference =
            burst::difference
            (
                boost::make_iterator_range(empty_set),
                boost::make_iterator_range(set)
            );

        BOOST_CHECK(difference.empty());
    }

    BOOST_AUTO_TEST_CASE(difference_of_saw_toothed_ranges_results_minuend)
    {
        const auto    minuend = burst::make_list({'h',      'f',      'd',      'b'     });
        const auto subtrahend = burst::make_list({     'g',      'e',      'c',      'a'});
        //                                         ^         ^         ^         ^

        const auto difference =
            burst::difference
            (
                boost::make_iterator_range(minuend),
                boost::make_iterator_range(subtrahend),
                std::greater<>()
            );

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            difference.begin(), difference.end(),
            minuend.begin(), minuend.end()
        );
    }

    BOOST_AUTO_TEST_CASE(difference_equals_minuend_if_subtrahend_surrounds_minuend)
    {
        const auto    minuend =   burst::make_list({       4, 6, 8       });
        const auto subtrahend = burst::make_vector({-1, 2,         10, 11});
        //                                                 ^  ^  ^

        const auto difference =
            burst::difference
            (
                boost::make_iterator_range(minuend),
                boost::make_iterator_range(subtrahend)
            );

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            difference.begin(), difference.end(),
            minuend.begin(), minuend.end()
        );
    }

    BOOST_AUTO_TEST_CASE(accepts_ranges_of_different_types)
    {
        const auto    minuend =   burst::make_list({0,    4,    6,    7, 9, 10    });
        const auto subtrahend = burst::make_vector({0, 2,    5, 6, 6,       10, 11});
        //                                                ^           ^  ^

        const auto difference =
            burst::difference
            (
                boost::make_iterator_range(minuend),
                boost::make_iterator_range(subtrahend)
            );

        const auto expected = burst::make_vector({4, 7, 9});
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            difference.begin(), difference.end(),
            expected.begin(), expected.end()
        );
    }

    BOOST_AUTO_TEST_CASE(resulting_range_is_writable)
    {
        auto minuend = burst::make_vector({1, 3, 5, 7, 9});
        auto subtrahend = burst::make_vector({2, 3, 4, 5});

        auto difference =
            burst::difference
            (
                boost::make_iterator_range(minuend),
                boost::make_iterator_range(subtrahend)
            );

        boost::for_each(difference, [] (auto & x) { x *= 2; });

        const auto expected = burst::make_vector({2, 14, 18});
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            difference.begin(), difference.end(),
            expected.begin(), expected.end()
        );
    }
BOOST_AUTO_TEST_SUITE_END()
