#include <iterator>
#include <limits>
#include <memory>
#include <vector>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/algorithm/sorting/counting_sort.hpp>

BOOST_AUTO_TEST_SUITE(counting_sort)
    BOOST_AUTO_TEST_CASE(sorting_already_sorted_range_results_the_same_range)
    {
        std::vector<std::uint8_t> values{0, 1, 2, 3, 4};

        std::vector<std::uint8_t> sorted_values(values.size());
        burst::counting_sort_copy(values.begin(), values.end(), sorted_values.begin());

        BOOST_CHECK(std::is_sorted(sorted_values.begin(), sorted_values.end()));
    }

    BOOST_AUTO_TEST_CASE(sorting_descending_range_results_ascending_range)
    {
        std::vector<std::size_t> values{0x0104, 0x0203, 0x0302, 0x0401};

        std::vector<std::size_t> sorted_values(values.size());
        burst::counting_sort_copy(values.begin(), values.end(), sorted_values.begin(),
            [] (const std::size_t & integer) -> std::uint8_t
            {
                return integer & 0xff;
            });

        std::vector<std::size_t> expected{0x0401, 0x0302, 0x0203, 0x0104};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(sorted_values), std::end(sorted_values),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(can_sort_bitwise)
    {
        std::vector<std::uint8_t> numbers{0, 5, 3, 7, 1, 2, 4, 6};

        std::vector<std::uint8_t> sorted_numbers(numbers.size());
        burst::counting_sort_copy(numbers.begin(), numbers.end(), sorted_numbers.begin(),
            [] (const std::uint8_t & number) -> bool
            {
                return number & 0x01;
            });

        // Порядок внутри чётных и нечётных групп сохраняется, потому что сортировка устойчива.
        std::vector<std::uint8_t> even_goes_first{0, 2, 4, 6, 5, 3, 7, 1};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(sorted_numbers), std::end(sorted_numbers),
            std::begin(even_goes_first), std::end(even_goes_first)
        );
    }

    BOOST_AUTO_TEST_CASE(sorting_chaotic_range_results_sorted_range)
    {
        std::vector<std::uint8_t> values{0x12, 0xfd, 0x00, 0x15, 0x66};

        std::vector<std::uint8_t> sorted_values(values.size());
        burst::counting_sort_copy(values.begin(), values.end(), sorted_values.begin());

        std::vector<std::uint8_t> expected{0x00, 0x12, 0x15, 0x66, 0xfd};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(sorted_values), std::end(sorted_values),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(negative_values_are_sorted_correctly)
    {
        std::vector<std::int8_t> values{0, -1, 1, -2, 2};

        std::vector<std::int8_t> sorted_values(values.size());
        burst::counting_sort_copy(values.begin(), values.end(), sorted_values.begin());

        std::vector<std::int8_t> expected{-2, -1, 0, 1, 2};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(sorted_values), std::end(sorted_values),
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

        std::vector<std::int8_t> sorted_values(values.size());
        burst::counting_sort_copy(values.begin(), values.end(), sorted_values.begin());

        std::vector<std::int8_t> expected
        {
            std::numeric_limits<std::int8_t>::min(),
            0,
            std::numeric_limits<std::int8_t>::max()
        };
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(sorted_values), std::end(sorted_values),
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

        std::vector<std::uint32_t> sorted_values(values.size());
        burst::counting_sort_copy(values.begin(), values.end(), sorted_values.begin(),
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
            std::begin(sorted_values), std::end(sorted_values),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(returns_iterator_past_the_end_of_the_sorted_range)
    {
        std::vector<std::uint8_t> initial{5, 4, 3, 2, 1};

        std::vector<std::uint8_t> sorted(initial.size() * 2);
        auto sorted_end = burst::counting_sort_copy(initial.begin(), initial.end(), sorted.begin());

        BOOST_CHECK((sorted_end == sorted.begin() + std::distance(initial.begin(), initial.end())));
    }

    BOOST_AUTO_TEST_CASE(initial_range_does_not_change)
    {
        std::vector<std::uint8_t> numbers{5, 4, 3, 2, 1};
        const auto numbers_copy = numbers;

        std::vector<std::uint8_t> sorted_numbers(numbers.size());
        burst::counting_sort_copy(numbers.begin(), numbers.end(), sorted_numbers.begin());

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(numbers), std::end(numbers),
            std::begin(numbers_copy), std::end(numbers_copy)
        );
    }

    BOOST_AUTO_TEST_CASE(can_sort_noncopyable_objects)
    {
        std::vector<std::unique_ptr<std::int8_t>> unsorted;
        unsorted.emplace_back(std::make_unique<std::int8_t>(3));
        unsorted.emplace_back(std::make_unique<std::int8_t>(5));
        unsorted.emplace_back(std::make_unique<std::int8_t>(-1));
        unsorted.emplace_back(std::make_unique<std::int8_t>(20));
        unsorted.emplace_back(std::make_unique<std::int8_t>(0));

        std::vector<std::unique_ptr<std::int8_t>> sorted(unsorted.size());
        burst::counting_sort_move(unsorted.begin(), unsorted.end(), sorted.begin(),
            [] (const auto & p)
            {
                return *p;
            });

        BOOST_CHECK(boost::algorithm::all_of(unsorted, [] (const auto & p) { return p == nullptr; }));

        auto expected = {-1, 0, 3, 5, 20};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::make_indirect_iterator(std::begin(sorted)), boost::make_indirect_iterator(std::end(sorted)),
            std::begin(expected), std::end(expected)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
