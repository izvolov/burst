#include <limits>
#include <vector>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/rbegin.hpp>
#include <boost/range/rend.hpp>
#include <boost/test/unit_test.hpp>

#include <thrust/algorithm/sorting/counting_sort.hpp>

BOOST_AUTO_TEST_SUITE(counting_sort)
    BOOST_AUTO_TEST_CASE(sorting_empty_range_does_nothing)
    {
        std::vector<std::uint8_t> values;

        std::vector<std::uint8_t> sorted_values(values.size());
        thrust::counting_sort(values.begin(), values.end(), sorted_values.begin());

        BOOST_CHECK(values == sorted_values);
    }

    BOOST_AUTO_TEST_CASE(sorting_already_sorted_range_results_the_same_range)
    {
        std::vector<std::uint8_t> initial{0, 1, 2, 3, 4};

        std::vector<std::uint8_t> sorted(initial.size());
        thrust::counting_sort(initial.begin(), initial.end(), sorted.begin());

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(sorted), boost::end(sorted),
            boost::begin(initial), boost::end(initial)
        );
    }

    BOOST_AUTO_TEST_CASE(sorting_descending_range_results_ascending_range)
    {
        std::vector<std::size_t> descending_by_low_byte{0x0104, 0x0203, 0x0302, 0x0401};

        std::vector<std::size_t> ascending_by_low_byte(descending_by_low_byte.size());
        thrust::counting_sort
        (
            descending_by_low_byte.begin(),
            descending_by_low_byte.end(),
            ascending_by_low_byte.begin(),
            [] (const std::size_t & integer) -> std::uint8_t { return integer & 0xff; }
        );

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::rbegin(descending_by_low_byte), boost::rend(descending_by_low_byte),
            boost::begin(ascending_by_low_byte), boost::end(ascending_by_low_byte)
        );
    }

    BOOST_AUTO_TEST_CASE(sorting_chaotic_range_results_sorted_range)
    {
        std::vector<std::uint8_t> chaos{0x12, 0xfd, 0x00, 0x15, 0x66};

        std::vector<std::uint8_t> sorted(chaos.size());
        thrust::counting_sort(chaos.begin(), chaos.end(), sorted.begin());

        std::vector<std::uint8_t> expected{0x00, 0x12, 0x15, 0x66, 0xfd};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(sorted), boost::end(sorted),
            boost::begin(expected), boost::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(negative_values_are_sorted_correctly)
    {
        std::vector<std::int8_t> unsorted{0, -1, 1, -2, 2};

        std::vector<std::int8_t> sorted(unsorted.size());
        thrust::counting_sort(unsorted.begin(), unsorted.end(), sorted.begin());

        std::vector<std::int8_t> expected{-2, -1, 0, 1, 2};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(sorted), boost::end(sorted),
            boost::begin(expected), boost::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(extreme_values_are_sorted_properly)
    {
        std::vector<std::int8_t> unsorted
        {
            0,
            std::numeric_limits<std::int8_t>::min(),
            std::numeric_limits<std::int8_t>::max()
        };

        std::vector<std::int8_t> sorted(unsorted.size());
        thrust::counting_sort(unsorted.begin(), unsorted.end(), sorted.begin());

        std::vector<std::int8_t> expected
        {
            std::numeric_limits<std::int8_t>::min(),
            0,
            std::numeric_limits<std::int8_t>::max()
        };
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(sorted), boost::end(sorted),
            boost::begin(expected), boost::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(sorting_algorithm_is_stable)
    {
        std::vector<std::uint16_t> unsorted
        {
            0x3301,
            0x3300,
            0x2201,
            0x2200,
            0x1101,
            0x1100
        };

        std::vector<std::uint16_t> sorted(unsorted.size());
        thrust::counting_sort
        (
            unsorted.begin(),
            unsorted.end(),
            sorted.begin(),
            [] (const std::uint16_t & integer) -> std::uint8_t { return integer & 0xff; }
        );

        std::vector<std::uint16_t> expected
        {
            0x3300,
            0x2200,
            0x1100,
            0x3301,
            0x2201,
            0x1101
        };
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(sorted), boost::end(sorted),
            boost::begin(expected), boost::end(expected)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
