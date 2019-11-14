#include <burst/container/make_forward_list.hpp>
#include <burst/iterator/difference_iterator.hpp>

#include <doctest/doctest.h>

#include <boost/range/iterator_range.hpp>

#include <forward_list>
#include <iterator>

TEST_SUITE("difference_iterator")
{
    TEST_CASE("difference_iterator_end_is_created_using_special_tag")
    {
        const auto minuend = {1, 2, 2, 3};
        const auto subtrahend = {0, 2, 2, 4};

        const auto difference_begin = burst::make_difference_iterator(minuend, subtrahend);
        const auto   difference_end = burst::make_difference_iterator(burst::iterator::end_tag, difference_begin);

        const auto expected_collection = {1, 3};
        CHECK(boost::make_iterator_range(difference_begin, difference_end) == expected_collection);
    }

    TEST_CASE("difference_iterator_accepts_forward_iterator")
    {
        const auto minuend = burst::make_forward_list({3, 5, 7});
        const auto subtrahend = burst::make_forward_list({1, 2, 3});

        const auto difference_begin = burst::make_difference_iterator(minuend, subtrahend);
        const auto   difference_end = burst::make_difference_iterator(burst::iterator::end_tag, difference_begin);

        auto expected_collection = {5, 7};
        CHECK(boost::make_iterator_range(difference_begin, difference_end) == expected_collection);
    }

    TEST_CASE("accepts_ranges_by_rvalue")
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
            burst::make_difference_iterator(burst::iterator::end_tag, difference_begin);

        auto expected_collection = {5, 7};
        CHECK(boost::make_iterator_range(difference_begin, difference_end) == expected_collection);
    }
}
