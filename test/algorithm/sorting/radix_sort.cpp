#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/rbegin.hpp>
#include <boost/range/rend.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/algorithm/sorting/radix_sort.hpp>
#include <test/output.hpp>

BOOST_AUTO_TEST_SUITE(radix_sort)
    BOOST_AUTO_TEST_CASE(sorting_empty_range_does_nothing)
    {
        std::vector<std::size_t> values;

        auto sorted_values = values;
        burst::radix_sort(sorted_values.begin(), sorted_values.end());

        BOOST_CHECK(sorted_values == values);
    }

    BOOST_AUTO_TEST_CASE(sorting_already_sorted_range_results_the_same_range)
    {
        std::vector<std::uint8_t> initial{0, 1, 2, 3, 4};

        auto sorted = initial;
        burst::radix_sort(sorted.begin(), sorted.end());

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(sorted), boost::end(sorted),
            boost::begin(initial), boost::end(initial)
        );
    }

    BOOST_AUTO_TEST_CASE(sorting_descending_range_results_ascending_range)
    {
        std::vector<std::string> descending{"1000", "100", "10", "1"};

        auto ascending = descending;
        burst::radix_sort(ascending.begin(), ascending.end(),
            [] (const std::string & string)
            {
                return string.size();
            }
        );

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::rbegin(descending), boost::rend(descending),
            boost::begin(ascending), boost::end(ascending)
        );
    }

    BOOST_AUTO_TEST_CASE(sorting_chaotic_range_results_sorted_range)
    {
        std::vector<std::uint32_t> numbers{100500, 42, 99999, 1000, 0};

        burst::radix_sort(numbers.begin(), numbers.end());

        std::vector<std::uint32_t> expected{0, 42, 1000, 99999, 100500};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(numbers), boost::end(numbers),
            boost::begin(expected), boost::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(can_sort_bitwise)
    {
        std::vector<std::uint8_t> numbers{0, 5, 3, 7, 1, 2, 4, 6};

        burst::radix_sort(numbers.begin(), numbers.end(), burst::identity<>(),
            [] (const std::uint8_t & number) -> bool
            {
                return number & 0x01;
            });

        std::vector<std::uint8_t> even_goes_first{0, 1, 2, 3, 4, 5, 6, 7};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(numbers), boost::end(numbers),
            boost::begin(even_goes_first), boost::end(even_goes_first)
        );
    }

    BOOST_AUTO_TEST_CASE(extreme_values_are_sorted_properly)
    {
        std::vector<std::size_t> numbers
        {
            0,
            std::numeric_limits<std::size_t>::min(),
            std::numeric_limits<std::size_t>::max()
        };

        burst::radix_sort(numbers.begin(), numbers.end());

        std::vector<std::size_t> expected
        {
            std::numeric_limits<std::size_t>::min(),
            0,
            std::numeric_limits<std::size_t>::max()
        };
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(numbers), boost::end(numbers),
            boost::begin(expected), boost::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(sorting_algorithm_is_stable)
    {
        std::vector<std::pair<std::uint32_t, std::string>> numbers
        {
            {1, "раз"},
            {1, "два"},
            {0, "три"},
            {0, "четыре"},
            {1, "пять"}
        };

        burst::radix_sort(numbers.begin(), numbers.end(),
            [] (const auto & pair)
            {
                return pair.first;
            });

        std::vector<std::pair<std::uint32_t, std::string>> expected
        {
            {0, "три"},
            {0, "четыре"},
            {1, "раз"},
            {1, "два"},
            {1, "пять"}
        };
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(numbers), boost::end(numbers),
            boost::begin(expected), boost::end(expected)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
