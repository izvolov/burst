#include <iterator>
#include <vector>

#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/iterator/join_iterator.hpp>

BOOST_AUTO_TEST_SUITE(join_iterator)
    BOOST_AUTO_TEST_CASE(join_iterator_end_is_created_using_special_tag)
    {
        auto  first = {500, 100};
        auto second = {600, 200};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto joined_begin = burst::make_join_iterator(ranges);
        auto   joined_end = burst::make_join_iterator(ranges, burst::iterator::end_tag);

        auto expected_collection = {500, 100, 600, 200};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            joined_begin, joined_end,
            std::begin(expected_collection), std::end(expected_collection)
        );
    }

    BOOST_AUTO_TEST_CASE(join_iterator_created_from_random_access_ranges_is_random_access_iterator)
    {
        auto  first = {3, 2};
        auto second = {1, 0};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto joined_begin = burst::make_join_iterator(ranges);
        auto joined_end   = burst::make_join_iterator(ranges, burst::iterator::end_tag);

        BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<decltype(joined_begin)>));
        BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<decltype(joined_end)>));
    }

    BOOST_AUTO_TEST_CASE(decrementing_random_access_join_iterator_end_is_legal)
    {
        auto  first = {3, 2};
        auto second = {1, 0};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto joined_end = burst::make_join_iterator(ranges, burst::iterator::end_tag);
        --joined_end;

        BOOST_CHECK_EQUAL(*joined_end, 0);
    }

    BOOST_AUTO_TEST_CASE(modifying_a_copy_of_join_iterator_does_not_affect_the_original_iterator)
    {
        std::vector<int> first{100, 50};
        std::vector<int> second{70, 30};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto join_iterator = burst::make_join_iterator(ranges);

        auto join_iterator_copy = join_iterator;
        join_iterator_copy += 2;

        BOOST_CHECK_EQUAL(*join_iterator_copy, 70);
        BOOST_CHECK_EQUAL(*join_iterator, 100);
    }

    BOOST_AUTO_TEST_CASE(random_accessing_near_range_bounds_is_correct)
    {
        auto  first = {3, 2};
        auto second = {1, 0};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto joined_begin = burst::make_join_iterator(ranges);
        auto joined_end   = burst::make_join_iterator(ranges, burst::iterator::end_tag);

        joined_begin += 2;
        BOOST_CHECK_EQUAL(*joined_begin, 1);
        BOOST_CHECK_EQUAL(joined_end - joined_begin, 2);

        joined_begin -= 2;
        BOOST_CHECK_EQUAL(*joined_begin, 3);
        BOOST_CHECK_EQUAL(joined_end - joined_begin, 4);
    }
BOOST_AUTO_TEST_SUITE_END()
