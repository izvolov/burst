#include <burst/container/make_vector.hpp>
#include <burst/iterator/join_iterator.hpp>
#include <burst/range/make_range_vector.hpp>

#include <doctest/doctest.h>

#include <boost/range/iterator_range.hpp>

#include <iterator>

TEST_SUITE("join_iterator")
{
    TEST_CASE("join_iterator_end_is_created_from_begin_using_special_tag")
    {
        auto  first = {500, 100};
        auto second = {600, 200};
        auto ranges = burst::make_range_vector(first, second);

        auto joined_begin = burst::make_join_iterator(ranges);
        auto   joined_end = burst::make_join_iterator(burst::iterator::end_tag, joined_begin);

        auto expected_collection = {500, 100, 600, 200};
        CHECK(boost::make_iterator_range(joined_begin, joined_end) == expected_collection);
    }

    TEST_CASE("join_iterator_created_from_random_access_ranges_is_random_access_iterator")
    {
        auto  first = {3, 2};
        auto second = {1, 0};
        auto ranges = burst::make_range_vector(first, second);

        auto joined_begin = burst::make_join_iterator(ranges);
        auto joined_end   = burst::make_join_iterator(burst::iterator::end_tag, joined_begin);

        BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<decltype(joined_begin)>));
        BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<decltype(joined_end)>));
    }

    TEST_CASE("decrementing_random_access_join_iterator_end_is_legal")
    {
        auto  first = {3, 2};
        auto second = {1, 0};
        auto ranges = burst::make_range_vector(first, second);

        auto joined_begin = burst::make_join_iterator(ranges);
        auto joined_end = burst::make_join_iterator(burst::iterator::end_tag, joined_begin);
        --joined_end;

        CHECK(*joined_end == 0);
    }

    TEST_CASE("modifying_a_copy_of_join_iterator_does_not_affect_the_original_iterator")
    {
        auto first = burst::make_vector({100, 50});
        auto second = burst::make_vector({70, 30});
        auto ranges = burst::make_range_vector(first, second);

        auto join_iterator = burst::make_join_iterator(ranges);

        auto join_iterator_copy = join_iterator;
        join_iterator_copy += 2;

        CHECK(*join_iterator_copy == 70);
        CHECK(*join_iterator == 100);
    }

    TEST_CASE("random_accessing_near_range_bounds_is_correct")
    {
        auto  first = {3, 2};
        auto second = {1, 0};
        auto ranges = burst::make_range_vector(first, second);

        auto joined_begin = burst::make_join_iterator(ranges);
        auto joined_end   = burst::make_join_iterator(burst::iterator::end_tag, joined_begin);

        joined_begin += 2;
        CHECK(*joined_begin == 1);
        CHECK(joined_end - joined_begin == 2);

        joined_begin -= 2;
        CHECK(*joined_begin == 3);
        CHECK(joined_end - joined_begin == 4);
    }
}
