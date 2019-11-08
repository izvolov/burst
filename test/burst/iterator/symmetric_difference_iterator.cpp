#include <burst/iterator/symmetric_difference_iterator.hpp>
#include <burst/range/make_range_vector.hpp>

#include <boost/test/unit_test.hpp>

#include <iterator>

BOOST_AUTO_TEST_SUITE(symmetric_difference_iterator)
    BOOST_AUTO_TEST_CASE(symmetric_difference_iterator_end_is_created_using_special_tag)
    {
        const auto  first = {   1, 2, 3   };
        const auto second = {0,    2,    4};
        const auto  third = {0,    2, 3   };
        //                      ^  ^     ^
        auto ranges = burst::make_range_vector(first, second, third);

        auto symmetric_difference_begin =
            burst::make_symmetric_difference_iterator(ranges);
        auto symmetric_difference_end =
            burst::make_symmetric_difference_iterator(burst::iterator::end_tag, symmetric_difference_begin);

        const auto expected_collection = {1, 2, 4};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            symmetric_difference_begin, symmetric_difference_end,
            std::begin(expected_collection), std::end(expected_collection)
        );
    }

    BOOST_AUTO_TEST_CASE(symmetric_difference_is_destructive_to_inner_ranges)
    {
        const auto  first = {1, 2, 3, 4         };
        const auto second = {      3, 4, 5, 6   };
        const auto  third = {         4, 5, 6, 7};

        auto ranges = burst::make_range_vector(first, second, third);

        const auto symmetric_difference_begin =
            burst::make_symmetric_difference_iterator(ranges);
        const auto symmetric_difference_end =
            burst::make_symmetric_difference_iterator(burst::iterator::end_tag, symmetric_difference_begin);

        // Проход по всем элементам симметрической разности.
        const auto size = std::distance(symmetric_difference_begin, symmetric_difference_end);
        BOOST_CHECK_EQUAL(size, 4);

        BOOST_CHECK(ranges[0] != first);
        BOOST_CHECK(ranges[1] != second);
        BOOST_CHECK(ranges[2] != third);
    }
BOOST_AUTO_TEST_SUITE_END()
