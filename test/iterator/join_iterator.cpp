#include <vector>
#include <list>

#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/iterator/join_iterator.hpp>
#include <burst/range/join.hpp>

BOOST_AUTO_TEST_SUITE(join_iterator)
    BOOST_AUTO_TEST_CASE(joining_empty_ranges_results_empty_range)
    {
        std::vector<int> first;
        std::vector<int> second;
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto joint_range = burst::join(ranges);

        BOOST_CHECK(joint_range.empty());
    }

    BOOST_AUTO_TEST_CASE(joining_one_range_results_the_same_range)
    {
        int array[] = {1, 2, 3, 4};
        auto ranges = {boost::make_iterator_range(array)};

        auto joint_range = burst::join(ranges);

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(array), boost::end(array),
            boost::begin(joint_range), boost::end(joint_range)
        );
    }

    BOOST_AUTO_TEST_CASE(overlaying_ranges_do_not_blend)
    {
        std::list<int>  first = {1, 2, 3};
        std::list<int> second = {2, 3, 4};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto joint_range = burst::join(ranges);

        auto expected_collection = {1, 2, 3, 2, 3, 4};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(joint_range), boost::end(joint_range),
            boost::begin(expected_collection), boost::end(expected_collection)
        );
    }

    BOOST_AUTO_TEST_CASE(joining_several_ranges_consecutively_puts_them_one_after_another)
    {
        auto  first = {'h', 'e'};
        auto second = {'l', 'l'};
        auto  third = {'o', '!'};
        auto ranges =
        {
            boost::make_iterator_range(first),
            boost::make_iterator_range(second),
            boost::make_iterator_range(third)
        };

        auto joint_range = burst::join(ranges);

        std::string expected_collection("hello!");
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(joint_range), boost::end(joint_range),
            boost::begin(expected_collection), boost::end(expected_collection)
        );
    }

    BOOST_AUTO_TEST_CASE(modifying_joint_mutable_ranges_is_allowed)
    {
        std::vector<int> first{100, 50};
        std::vector<int> second{70, 30};

        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto joint_range = burst::join(ranges);
        boost::for_each(joint_range, [] (int & x) { x /= 10; });

        BOOST_CHECK_EQUAL(first[0], 10);
        BOOST_CHECK_EQUAL(first[1], 5);
        BOOST_CHECK_EQUAL(second[0], 7);
        BOOST_CHECK_EQUAL(second[1], 3);
    }

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
            boost::begin(expected_collection), boost::end(expected_collection)
        );
    }

    BOOST_AUTO_TEST_CASE(join_iterator_created_from_random_access_ranges_is_random_access_iterator)
    {
        auto  first = {3, 2};
        auto second = {1, 0};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto joined_begin = burst::make_join_iterator(ranges);
        auto joined_end   = burst::make_join_iterator(ranges, burst::iterator::end_tag);

        joined_begin += 3;
        BOOST_CHECK_EQUAL(*joined_begin, 0);
        BOOST_CHECK_EQUAL(joined_end - joined_begin, 1);

        joined_begin -= 2;
        BOOST_CHECK_EQUAL(*joined_begin, 2);
        BOOST_CHECK_EQUAL(joined_end - joined_begin, 3);
    }

    BOOST_AUTO_TEST_CASE(random_access_joined_range_has_size_method)
    {
        std::vector<int>  first{1, 0};
        std::vector<int> second{3, 2};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto joint_range = burst::join(ranges);

        BOOST_CHECK_EQUAL(joint_range.size(), 4);
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

    BOOST_AUTO_TEST_CASE(joined_random_access_range_can_be_reversed)
    {
        std::string join("join");
        std::string iterator("iterator");
        std::string can("can");
        std::string be("be");
        std::string reversed("reversed");
        auto ranges =
        {
            boost::make_iterator_range(join),
            boost::make_iterator_range(iterator),
            boost::make_iterator_range(can),
            boost::make_iterator_range(be),
            boost::make_iterator_range(reversed)
        };

        auto joint_range = burst::join(ranges);
        auto reversed_range = boost::adaptors::reverse(joint_range);

        std::string expected = join + iterator + can + be + reversed;
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            reversed_range.begin(), reversed_range.end(),
            expected.rbegin(), expected.rend()
        );
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
