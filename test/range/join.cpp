#include <iterator>
#include <list>
#include <string>
#include <vector>

#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/range/join.hpp>

BOOST_AUTO_TEST_SUITE(join)
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
            std::begin(array), std::end(array),
            std::begin(joint_range), std::end(joint_range)
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
            std::begin(joint_range), std::end(joint_range),
            std::begin(expected_collection), std::end(expected_collection)
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
            std::begin(joint_range), std::end(joint_range),
            std::begin(expected_collection), std::end(expected_collection)
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

    BOOST_AUTO_TEST_CASE(join_function_accepts_inplace_initializer_list)
    {
        std::string hello("hello");
        std::string world("world");

        auto joint_range = burst::join
        ({
            boost::make_iterator_range(hello),
            boost::make_iterator_range(world)
        });

        std::string helloworld("helloworld");
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(helloworld), std::end(helloworld),
            std::begin(joint_range), std::end(joint_range)
        );
    }

    BOOST_AUTO_TEST_CASE(random_access_joined_range_has_size_method)
    {
        std::vector<int>  first{1, 0};
        std::vector<int> second{3, 2};
        auto ranges = {boost::make_iterator_range(first), boost::make_iterator_range(second)};

        auto joint_range = burst::join(ranges);

        BOOST_CHECK_EQUAL(joint_range.size(), 4);
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
            std::begin(reversed_range), std::end(reversed_range),
            std::rbegin(expected), std::rend(expected)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
