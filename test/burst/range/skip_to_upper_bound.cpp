#include <burst/container/make_vector.hpp>
#include <burst/range/skip_to_upper_bound.hpp>

#include <doctest/doctest.h>

#include <boost/range/iterator_range.hpp>

#include <functional>
#include <vector>

TEST_SUITE("skip_to_upper_bound")
{
    TEST_CASE("Пропуск в пустом диапазоне оставляет диапазон пустым")
    {
        std::vector<int> empty_container;
        auto empty_range = boost::make_iterator_range(empty_container);

        burst::skip_to_upper_bound(empty_range, 5);

        CHECK(empty_range.empty());
    }

    TEST_CASE("Пропуск до значения в упорядоченном диапазоне, содержащем это значение, продвигает "
        "начало диапазона на следующую позицию после последнего вхождения заданного значения")
    {
        auto values = burst::make_vector({1, 5, 5, 7});
        auto sorted_range = boost::make_iterator_range(values);

        burst::skip_to_upper_bound(sorted_range, 5);

        CHECK(sorted_range == boost::make_iterator_range(values).advance_begin(3));
    }

    TEST_CASE("Пропуск до значения, превышающего все элементы диапазона, оставляет диапазон пустым")
    {
        auto values = {1, 2, 3};
        auto sorted_range = boost::make_iterator_range(values);

        burst::skip_to_upper_bound(sorted_range, 100500);

        CHECK(sorted_range.empty());
    }

    TEST_CASE("Пропуск до значения, меньшего чем все элементы диапазона, не изменяет диапазон")
    {
        auto values = {3, 2, 1};
        auto sorted_range = boost::make_iterator_range(values);
        auto sorted_range_copy = sorted_range;

        burst::skip_to_upper_bound(sorted_range, 100500, std::greater<>{});

        CHECK(sorted_range == sorted_range_copy);
    }

    TEST_CASE("Пропуск до значения, отсутствующего в диапазоне, продвигает начало диапазона на "
        "первый из его элементов, который строго больше заданного значения")
    {
        std::vector<double> values{3.14, 2.71, 1.6};
        auto sorted_range = boost::make_iterator_range(values);

        burst::skip_to_upper_bound(sorted_range, 2.5, std::greater<>{});

        CHECK(sorted_range == boost::make_iterator_range(values).advance_begin(2));
    }
}
