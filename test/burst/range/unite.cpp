#include <burst/container/make_list.hpp>
#include <burst/range/make_range_vector.hpp>
#include <burst/range/unite.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>

#include <functional>
#include <string>
#include <vector>

TEST_SUITE("unite")
{
    TEST_CASE("Объединение пустых диапазонов — пустой диапазон")
    {
        std::vector<int> first;
        std::vector<int> second;
        auto ranges = burst::make_range_vector(first, second);

        auto range_union = burst::unite(ranges);

        CHECK(range_union.empty());
    }

    TEST_CASE("Объединение одинаковых диапазонов порождает диапазон, равный исходным")
    {
        auto first = {4, 5, 6, 7};
        auto second = first;
        auto ranges = burst::make_range_vector(first, second);

        auto range_union = burst::unite(ranges);

        CHECK(range_union == first);
    }

    TEST_CASE("Объединение одного диапазона — сам этот диапазон")
    {
        auto only = boost::irange(1, 5);
        auto ranges = burst::make_range_vector(only);
        auto range_union = burst::unite(ranges);

        CHECK(range_union == boost::irange(1, 5));
    }

    TEST_CASE("Объединение вложенных диапазонов порождает диапазон, равный длиннейшему из них")
    {
        std::string  long_range("abcdef");
        std::string short_range("cde");

        auto range_union = burst::unite(std::tie(long_range, short_range));

        CHECK(range_union == long_range);
    }

    TEST_CASE("Объединение пилообразно дополняющих друг друга диапазонов содержит все элементы "
        "исходных диапазонов")
    {
        auto first = burst::make_list({'h', 'f', 'd', 'b'});
        auto second = burst::make_list({'g', 'e', 'c', 'a'});
        auto ranges = burst::make_range_vector(first, second);

        auto range_union = burst::unite(ranges, std::greater<>{});

        CHECK(range_union == std::string("hgfedcba"));
    }

    TEST_CASE("Объединение накладывающихся диапазонов не содержит повторяющиеся элементы")
    {
        auto  first = {3, 2, 1};
        auto second = {4, 3, 2};
        auto  third = {5, 3, 1};
        auto ranges = burst::make_range_vector(first, second, third);

        auto range_union = burst::unite(ranges, std::greater<>{});

        auto expected_collection = {5, 4, 3, 2, 1};
        CHECK(range_union == expected_collection);
    }

    TEST_CASE("Работает с диапазонами как с мультимножествами")
    {
        auto  first = {1, 1, 1, 2, 2, 3};
        auto second = {1, 1, 2};
        auto  third = {1};
        auto ranges = burst::make_range_vector(first, second, third);

        auto range_union = burst::unite(ranges);

        auto expected_collection = {1, 1, 1, 2, 2, 3};
        CHECK(range_union == expected_collection);
    }

    TEST_CASE("Объединение можно вызывать без предварительного создания диапазона диапазонов")
    {
        const auto  first = {5, 5, 5, 4, 4, 3};
        const auto second = {5, 5, 4, 4};
        const auto  third = {5, 5};

        auto range_union = burst::unite(std::tie(first, second, third), std::greater<>{});

        auto expected_collection = {5, 5, 5, 4, 4, 3};
        CHECK(range_union == expected_collection);
    }
}
