#include <burst/container/make_forward_list.hpp>
#include <burst/iterator/difference_iterator.hpp>

#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <forward_list>
#include <iterator>

BOOST_AUTO_TEST_SUITE(difference_iterator)
    BOOST_AUTO_TEST_CASE(difference_iterator_end_is_created_using_special_tag)
    {
        const auto first = {1, 2, 2, 3};
        const auto second = {0, 2, 2, 4};
        const auto minuend = boost::make_iterator_range(first);
        const auto subtrahend = boost::make_iterator_range(second);

        const auto difference_begin = burst::make_difference_iterator(minuend, subtrahend);
        const auto   difference_end = burst::make_difference_iterator(difference_begin, burst::iterator::end_tag);

        const auto expected_collection = {1, 3};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            difference_begin, difference_end,
            std::begin(expected_collection), std::end(expected_collection)
        );
    }

    BOOST_AUTO_TEST_CASE(difference_iterator_accepts_forward_iterator)
    {
        const auto first = burst::make_forward_list({3, 5, 7});
        const auto second = burst::make_forward_list({1, 2, 3});
        const auto minuend = boost::make_iterator_range(first);
        const auto subtrahend = boost::make_iterator_range(second);

        const auto difference_begin = burst::make_difference_iterator(minuend, subtrahend);
        const auto   difference_end = burst::make_difference_iterator(difference_begin, burst::iterator::end_tag);

        auto expected_collection = {5, 7};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            difference_begin, difference_end,
            std::begin(expected_collection), std::end(expected_collection)
        );
    }

    BOOST_AUTO_TEST_CASE(accepts_ranges_by_rvalue)
    {
        const auto minuend = burst::make_forward_list({3, 5, 7});
        const auto subtrahend = burst::make_forward_list({1, 2, 3});

        const auto difference_begin =
            burst::make_difference_iterator
            (
                boost::make_iterator_range(minuend),
                boost::make_iterator_range(subtrahend)
            );
        const auto difference_end =
            burst::make_difference_iterator(difference_begin, burst::iterator::end_tag);

        auto expected_collection = {5, 7};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            difference_begin, difference_end,
            std::begin(expected_collection), std::end(expected_collection)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
