#include <limits>
#include <forward_list>
#include <string>
#include <utility>
#include <vector>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/rbegin.hpp>
#include <boost/range/rend.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/algorithm/sorting/radix_sort_copy.hpp>
#include <test/output.hpp>

BOOST_AUTO_TEST_SUITE(radix_sort_copy)
    BOOST_AUTO_TEST_CASE(sorting_empty_range_does_nothing)
    {
        std::vector<std::size_t> values;

        std::vector<std::size_t> sorted_values(values.size());
        burst::radix_sort_copy(values.begin(), values.end(), sorted_values.begin());

        BOOST_CHECK(sorted_values == values);
    }

    BOOST_AUTO_TEST_CASE(sorting_already_sorted_range_results_the_same_range)
    {
        std::vector<std::uint8_t> initial{0, 1, 2, 3, 4};

        std::vector<std::uint8_t> sorted(initial.size());
        burst::radix_sort_copy(initial.begin(), initial.end(), sorted.begin());

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(sorted), boost::end(sorted),
            boost::begin(initial), boost::end(initial)
        );
    }

    BOOST_AUTO_TEST_CASE(sorting_descending_range_results_ascending_range)
    {
        std::vector<std::string> descending{"1000", "100", "10", "1"};

        std::vector<std::string> ascending(descending.size());
        burst::radix_sort_copy(descending.begin(), descending.end(), ascending.begin(),
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
        std::vector<std::uint32_t> chaos{100500, 42, 99999, 1000, 0};

        std::forward_list<std::uint32_t> order(chaos.size());
        burst::radix_sort_copy(chaos.begin(), chaos.end(), order.begin());

        std::vector<std::uint32_t> expected{0, 42, 1000, 99999, 100500};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(order), boost::end(order),
            boost::begin(expected), boost::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(can_sort_bitwise)
    {
        std::vector<std::uint8_t> unsorted{0, 5, 3, 7, 1, 2, 4, 6};

        std::forward_list<std::uint8_t> sorted(unsorted.size());
        burst::radix_sort_copy(unsorted.begin(), unsorted.end(), sorted.begin(), burst::identity<>(),
            [] (const std::uint8_t & number) -> bool
            {
                return number & 0x01;
            });

        std::vector<std::uint8_t> even_goes_first{0, 1, 2, 3, 4, 5, 6, 7};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(sorted), boost::end(sorted),
            boost::begin(even_goes_first), boost::end(even_goes_first)
        );
    }

    BOOST_AUTO_TEST_CASE(extreme_values_are_sorted_properly)
    {
        std::vector<std::size_t> unsorted
        {
            0,
            std::numeric_limits<std::size_t>::min(),
            std::numeric_limits<std::size_t>::max()
        };

        std::vector<std::size_t> sorted(unsorted.size());
        burst::radix_sort_copy(unsorted.begin(), unsorted.end(), sorted.begin());

        std::vector<std::size_t> expected
        {
            std::numeric_limits<std::size_t>::min(),
            0,
            std::numeric_limits<std::size_t>::max()
        };
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(sorted), boost::end(sorted),
            boost::begin(expected), boost::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(sorting_algorithm_is_stable)
    {
        std::vector<std::pair<std::uint32_t, std::string>> unsorted
        {
            {1, "раз"},
            {1, "два"},
            {0, "три"},
            {0, "четыре"},
            {1, "пять"}
        };

        auto sorted = unsorted;
        burst::radix_sort_copy(unsorted.begin(), unsorted.end(), sorted.begin(),
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
            boost::begin(sorted), boost::end(sorted),
            boost::begin(expected), boost::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(initial_range_does_not_change)
    {
        std::vector<std::size_t> initial{5, 4, 3, 2, 1};
        auto initial_copy = initial;

        std::vector<std::size_t> sorted;
        burst::radix_sort_copy(initial.begin(), initial.end(), std::back_inserter(sorted));
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(initial), boost::end(initial),
            boost::begin(initial_copy), boost::end(initial_copy)
        );
    }

    BOOST_AUTO_TEST_CASE(sorted_range_is_exactly_the_same_size_as_initial_range)
    {
        std::vector<std::size_t> initial{5, 4, 3, 2, 1};
        std::vector<std::size_t> expected{1, 2, 3, 4, 5};

        // rar = random access range.
        {
            std::vector<std::size_t> rar(initial.size() * 2, 0);
            auto sorted_rar_end = burst::radix_sort_copy(initial.begin(), initial.end(), rar.begin());
            BOOST_CHECK_EQUAL_COLLECTIONS
            (
                boost::begin(rar), sorted_rar_end,
                boost::begin(expected), boost::end(expected)
            );
            BOOST_CHECK
            (
                boost::make_iterator_range(sorted_rar_end, rar.end()) == std::vector<std::size_t>(initial.size(), 0)
            );
        }

        // fr = forward range.
        {
            std::forward_list<std::size_t> fr(initial.size() * 2, 0);
            auto sorted_fr_end = burst::radix_sort_copy(initial.begin(), initial.end(), fr.begin());
            BOOST_CHECK_EQUAL_COLLECTIONS
            (
                boost::begin(fr), sorted_fr_end,
                boost::begin(expected), boost::end(expected)
            );
            BOOST_CHECK
            (
                boost::make_iterator_range(sorted_fr_end, fr.end()) == std::vector<std::size_t>(initial.size(), 0)
            );
        }
    }
BOOST_AUTO_TEST_SUITE_END()
