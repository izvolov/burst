#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/make_range_vector.hpp>
#include <burst/range/merge.hpp>

#include <doctest/doctest.h>

#include <boost/range/algorithm/for_each.hpp>

#include <functional>
#include <vector>

TEST_SUITE("merge")
{
    TEST_CASE("Слияние пустых диапазонов — пустой диапазон")
    {
        std::vector<int> first;
        std::vector<int> second;
        auto ranges = burst::make_range_vector(first, second);

        auto merged_range = burst::merge(ranges);
        CHECK(merged_range.empty());
    }

    TEST_CASE("Слияние одного диапазона — сам этот диапазон")
    {
        int array[] = {1, 2, 3, 4};
        auto ranges = burst::make_range_vector(array);

        auto merged_range = burst::merge(ranges);
        CHECK(merged_range == array);
    }

    TEST_CASE("Слияние пилообразно дополняющих друг друга диапазонов содержит все элементы "
        "исходных диапазонов")
    {
        auto first = burst::make_list({'h', 'f', 'd', 'b'});
        auto second = burst::make_list({'g', 'e', 'c', 'a'});
        auto ranges = burst::make_range_vector(first, second);

        auto merged_range = burst::merge(ranges, std::greater<>{});

        auto expected_collection = {'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a'};
        CHECK(merged_range == expected_collection);
    }

    TEST_CASE("Слияние нескольких непересекающихся диапазонов порождает диапазон, составленный из "
        "всех элементов исходных диапазонов")
    {
        auto  first = {1, 2, 3};
        auto second = {4, 5, 6};
        auto  third = {7, 8, 9};
        auto ranges = burst::make_range_vector(first, second, third);

        auto merged_range = burst::merge(ranges, std::less<>{});

        auto expected_collection = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        CHECK(merged_range == expected_collection);
    }

    TEST_CASE("Результирующий диапазон ссылается на исходные диапазоны, так что изменение "
        "элементов результирующего диапазона влечёт изменение соответствующих элементов исходных")
    {
        auto first = burst::make_vector({100, 50});
        auto second = burst::make_vector({70, 30});

        auto ranges = burst::make_range_vector(first, second);

        auto merged_range = burst::merge(ranges, std::greater<>{});
        boost::for_each(merged_range, [] (auto & x) { x /= 10; });

        CHECK(first[0] == 10);
        CHECK(first[1] == 5);
        CHECK(second[0] == 7);
        CHECK(second[1] == 3);
    }
}
