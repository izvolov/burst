#include <vector>
#include <list>

#include <boost/range/iterator_range.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/test/unit_test.hpp>

#include <thrust/iterator/join_iterator.hpp>
#include <thrust/range/join.hpp>

BOOST_AUTO_TEST_SUITE(join_iterator)
    BOOST_AUTO_TEST_CASE(joining_empty_ranges_results_empty_range)
    {
        std::vector<int> first;
        std::vector<int> second;
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto joint_range = thrust::join(ranges);

        BOOST_CHECK(joint_range.empty());
    }

    BOOST_AUTO_TEST_CASE(joining_one_range_results_the_same_range)
    {
        int array[] = {1, 2, 3, 4};
        auto ranges = {boost::make_iterator_range(array)};

        auto joint_range = thrust::join(ranges);

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

        auto joint_range = thrust::join(ranges);

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

        auto joint_range = thrust::join(ranges);

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

        auto joint_range = thrust::join(ranges);
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

        auto joined_begin = thrust::make_join_iterator(ranges);
        auto   joined_end = thrust::make_join_iterator(ranges, thrust::iterator::end_tag);

        auto expected_collection = {500, 100, 600, 200};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            joined_begin, joined_end,
            boost::begin(expected_collection), boost::end(expected_collection)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
