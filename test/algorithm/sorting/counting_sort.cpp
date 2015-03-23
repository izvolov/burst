#include <iterator>
#include <limits>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <burst/algorithm/sorting/counting_sort.hpp>

BOOST_AUTO_TEST_SUITE(counting_sort)
    BOOST_AUTO_TEST_CASE(sorting_empty_range_does_nothing)
    {
        std::vector<std::uint8_t> values;

        burst::counting_sort(values.begin(), values.end());

        BOOST_CHECK(values.empty());
    }

    BOOST_AUTO_TEST_CASE(sorting_already_sorted_range_results_the_same_range)
    {
        std::vector<std::uint8_t> values{0, 1, 2, 3, 4};

        burst::counting_sort(values.begin(), values.end());

        BOOST_CHECK(std::is_sorted(values.begin(), values.end()));
    }

    BOOST_AUTO_TEST_CASE(sorting_descending_range_results_ascending_range)
    {
        std::vector<std::size_t> values{0x0104, 0x0203, 0x0302, 0x0401};

        burst::counting_sort(values.begin(), values.end(),
            [] (const std::size_t & integer) -> std::uint8_t
            {
                return integer & 0xff;
            });

        std::vector<std::size_t> expected{0x0401, 0x0302, 0x0203, 0x0104};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(values), std::end(values),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(can_sort_bitwise)
    {
        std::vector<std::uint8_t> numbers{0, 5, 3, 7, 1, 2, 4, 6};

        burst::counting_sort(numbers.begin(), numbers.end(),
            [] (const std::uint8_t & number) -> bool
            {
                return number & 0x01;
            });

        // Порядок внутри чётных и нечётных групп сохраняется, потому что сортировка устойчива.
        std::vector<std::uint8_t> even_goes_first{0, 2, 4, 6, 5, 3, 7, 1};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(numbers), std::end(numbers),
            std::begin(even_goes_first), std::end(even_goes_first)
        );
    }

    BOOST_AUTO_TEST_CASE(sorting_chaotic_range_results_sorted_range)
    {
        std::vector<std::uint8_t> values{0x12, 0xfd, 0x00, 0x15, 0x66};

        burst::counting_sort(values.begin(), values.end());

        std::vector<std::uint8_t> expected{0x00, 0x12, 0x15, 0x66, 0xfd};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(values), std::end(values),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(negative_values_are_sorted_correctly)
    {
        std::vector<std::int8_t> values{0, -1, 1, -2, 2};

        burst::counting_sort(values.begin(), values.end());

        std::vector<std::int8_t> expected{-2, -1, 0, 1, 2};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(values), std::end(values),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(extreme_values_are_sorted_properly)
    {
        std::vector<std::int8_t> values
        {
            0,
            std::numeric_limits<std::int8_t>::min(),
            std::numeric_limits<std::int8_t>::max()
        };

        burst::counting_sort(values.begin(), values.end());

        std::vector<std::int8_t> expected
        {
            std::numeric_limits<std::int8_t>::min(),
            0,
            std::numeric_limits<std::int8_t>::max()
        };
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(values), std::end(values),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(sorting_algorithm_is_stable)
    {
        std::vector<std::uint32_t> values
        {
            0x3301,
            0x3300,
            0x2201,
            0x2200,
            0x1101,
            0x1100
        };

        burst::counting_sort(values.begin(), values.end(),
            [] (const std::uint32_t & integer) -> std::uint8_t
            {
                return integer & 0xff;
            });

        std::vector<std::uint32_t> expected
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
            std::begin(values), std::end(values),
            std::begin(expected), std::end(expected)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
