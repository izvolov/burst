#include <burst/iterator/union_iterator.hpp>
#include <burst/range/make_range_vector.hpp>

#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <forward_list>
#include <iterator>

BOOST_AUTO_TEST_SUITE(union_iterator)
    BOOST_AUTO_TEST_CASE(union_iterator_end_is_created_using_special_tag)
    {
        auto  first = {1, 2, 2, 3};
        auto second = {0, 2, 2, 4};
        auto ranges = burst::make_range_vector(first, second);

        auto union_begin = burst::make_union_iterator(boost::make_iterator_range(ranges));
        auto   union_end = burst::make_union_iterator(burst::iterator::end_tag, union_begin);

        auto expected_collection = {0, 1, 2, 2, 3, 4};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            union_begin, union_end,
            std::begin(expected_collection), std::end(expected_collection)
        );
    }

    BOOST_AUTO_TEST_CASE(union_iterator_accepts_forward_iterator)
    {
        std::forward_list<double> forward_range{1.6, 2.71, 3.14};
        auto ranges = burst::make_range_vector(forward_range);

        auto union_begin = burst::make_union_iterator(boost::make_iterator_range(ranges));
        auto   union_end = burst::make_union_iterator(burst::iterator::end_tag, union_begin);

        auto expected_collection = {1.6, 2.71, 3.14};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            union_begin, union_end,
            std::begin(expected_collection), std::end(expected_collection)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
