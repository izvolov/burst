#include <burst/algorithm/counting_sort.hpp>

#include <doctest/doctest.h>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/range/iterator_range.hpp>

#include <forward_list>
#include <iterator>
#include <limits>
#include <memory>
#include <vector>

TEST_SUITE("counting_sort")
{
    TEST_CASE("Сортировка отсортированного диапазона не изменяет его")
    {
        std::vector<std::uint8_t> values{0, 1, 2, 3, 4};

        std::vector<std::uint8_t> sorted_values(values.size());
        burst::counting_sort_copy(values.begin(), values.end(), sorted_values.begin());

        CHECK(std::is_sorted(sorted_values.begin(), sorted_values.end()));
    }

    TEST_CASE("Результат сортировки убывающего диапазона — возрастающий диапазон")
    {
        std::vector<std::size_t> values{0x0104, 0x0203, 0x0302, 0x0401};

        std::vector<std::size_t> sorted_values(values.size());
        burst::counting_sort_copy(values.begin(), values.end(), sorted_values.begin(),
            [] (const std::size_t & integer) -> std::uint8_t
            {
                return integer & 0xff;
            });

        std::vector<std::size_t> expected{0x0401, 0x0302, 0x0203, 0x0104};
        CHECK(sorted_values == expected);
    }

    TEST_CASE("Умеет сортировать побитово")
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
        CHECK(sorted_numbers == even_goes_first);
    }

    TEST_CASE("Сортировка хаоса приводит к порядку")
    {
        std::vector<std::uint8_t> values{0x12, 0xfd, 0x00, 0x15, 0x66};

        std::vector<std::uint8_t> sorted_values(values.size());
        burst::counting_sort_copy(values.begin(), values.end(), sorted_values.begin());

        std::vector<std::uint8_t> expected{0x00, 0x12, 0x15, 0x66, 0xfd};
        CHECK(sorted_values == expected);
    }

    TEST_CASE("Отрицательные значения предшествуют неотрицательным в результате сортировки")
    {
        std::vector<std::int8_t> values{0, -1, 1, -2, 2};

        std::vector<std::int8_t> sorted_values(values.size());
        burst::counting_sort_copy(values.begin(), values.end(), sorted_values.begin());

        std::vector<std::int8_t> expected{-2, -1, 0, 1, 2};
        CHECK(sorted_values == expected);
    }

    TEST_CASE("Экстремальные значения элементов сортируются корректно")
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
        CHECK(sorted_values == expected);
    }

    TEST_CASE("Алгоритм сортировки устойчив")
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
        CHECK(sorted_values == expected);
    }

    TEST_CASE("Возвращает итератор после последнего отсортированного элемента в результирующем "
        "диапазоне")
    {
        std::vector<std::uint8_t> initial{5, 4, 3, 2, 1};

        std::vector<std::uint8_t> sorted(initial.size() * 2);
        auto sorted_end = burst::counting_sort_copy(initial.begin(), initial.end(), sorted.begin());

        CHECK(sorted_end == sorted.begin() + std::distance(initial.begin(), initial.end()));
    }

    TEST_CASE("Исходный диапазон не изменяется")
    {
        std::vector<std::uint8_t> numbers{5, 4, 3, 2, 1};
        const auto numbers_copy = numbers;

        std::vector<std::uint8_t> sorted_numbers(numbers.size());
        burst::counting_sort_copy(numbers.begin(), numbers.end(), sorted_numbers.begin());

        CHECK(numbers == numbers_copy);
    }

    TEST_CASE("Может сортировать некопируемые элементы")
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

        CHECK(boost::algorithm::all_of(unsorted, [] (const auto & p) { return p == nullptr; }));

        auto expected = {-1, 0, 3, 5, 20};
        CHECK
        (
            boost::make_iterator_range(boost::make_indirect_iterator(std::begin(sorted)), boost::make_indirect_iterator(std::end(sorted))) ==
            expected
        );
    }

    TEST_CASE("Для сортировки достаточно однонаправленного итератора")
    {
        auto unsorted = std::vector<std::int8_t>{4, 3, 2, 1};
        auto forward_list = std::forward_list<std::int8_t>(unsorted.begin(), unsorted.end());

        std::vector<std::int8_t> sorted(unsorted.size(), 0);
        burst::counting_sort_copy(forward_list.begin(), forward_list.end(), sorted.begin());

        auto expected = std::vector<std::int8_t>{1, 2, 3, 4};
        CHECK(sorted == expected);
    }

    TEST_CASE("Допускает диапазоны на вход")
    {
        std::vector<std::uint8_t> values{5, 4, 3, 2, 1};

        std::vector<std::uint8_t> sorted_values(values.size());
        burst::counting_sort_copy(values, sorted_values.begin());

        std::vector<std::uint8_t> expected{1, 2, 3, 4, 5};
        CHECK(sorted_values == expected);
    }
}
