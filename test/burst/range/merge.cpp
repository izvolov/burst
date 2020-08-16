#include <burst/container/make_deque.hpp>
#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/make_range_vector.hpp>
#include <burst/range/merge.hpp>

#include <doctest/doctest.h>

#include <boost/range/algorithm/for_each.hpp>

#include <functional>
#include <sstream>
#include <tuple>
#include <vector>

namespace
{
    template <typename Value>
    auto make_istream_range (std::istream & s)
    {
        return
            boost::make_iterator_range
            (
                std::istream_iterator<Value>(s),
                std::istream_iterator<Value>{}
            );
    }
}

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

        auto merged_range = burst::merge(std::tie(first, second), std::greater<>{});
        boost::for_each(merged_range, [] (auto & x) { x /= 10; });

        CHECK(first[0] == 10);
        CHECK(first[1] == 5);
        CHECK(second[0] == 7);
        CHECK(second[1] == 3);
    }

    TEST_CASE("Слияние можно вызывать без предварительного создания диапазона диапазонов")
    {
        auto odd = burst::make_vector({1, 3, 5, 7});
        auto even = burst::make_vector({0, 2, 4, 6, 8});

        auto merged_range = burst::merge(std::tie(odd, even));

        auto expected_collection = {0, 1, 2, 3, 4, 5, 6, 7, 8};
        CHECK(merged_range == expected_collection);
    }

    TEST_CASE("Можно сливать диапазоны разных типов")
    {
        auto one = burst::make_vector({0, 3, 6, 9});
        auto two = burst::make_list({1, 4, 7, 10});
        auto three = burst::make_deque({2, 5, 8, 11});

        auto merged_range = burst::merge(std::tie(one, two, three));

        auto expected_collection = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
        CHECK(merged_range == expected_collection);
    }

    TEST_CASE("Можно сливать с предикатом диапазоны разных типов")
    {
        auto one = burst::make_vector({9, 6, 3, 0});
        auto two = burst::make_list({10, 7, 4, 1});
        auto three = burst::make_deque({11, 8, 5, 2});

        auto merged_range = burst::merge(std::tie(one, two, three), std::greater<>{});

        auto expected_collection = {11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        CHECK(merged_range == expected_collection);
    }

    TEST_CASE("Можно сливать однопроходные диапазоны")
    {
        auto first = std::stringstream("0 2 4 6");
        auto second = std::stringstream("1 3 5 7");

        auto one = make_istream_range<int>(first);
        auto two = make_istream_range<int>(second);

        auto merged_range = burst::merge(std::tie(one, two));

        auto expected_collection = {0, 1, 2, 3, 4, 5, 6, 7};
        CHECK(merged_range == expected_collection);
    }
}
