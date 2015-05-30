#include <iterator>

#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/iterator/semiintersect_iterator.hpp>

BOOST_AUTO_TEST_SUITE(semiintersect_iterator)
    BOOST_AUTO_TEST_CASE(semiintersect_iterator_end_is_created_using_special_tag)
    {
        auto  first = {1, 2, 3};
        auto second = {0, 2, 4};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto min_items_to_score = 2ul;
        auto semiintersected_begin = burst::make_semiintersect_iterator(ranges, min_items_to_score);
        auto   semiintersected_end = burst::make_semiintersect_iterator(ranges, burst::iterator::end_tag);

        auto expected_collection = {2};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            semiintersected_begin, semiintersected_end,
            std::begin(expected_collection), std::end(expected_collection)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
