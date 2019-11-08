#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/join.hpp>
#include <burst/range/make_range_vector.hpp>

#include <doctest/doctest.h>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/none_of.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/rbegin.hpp>
#include <boost/range/rend.hpp>

#include <algorithm>
#include <string>
#include <vector>

TEST_SUITE("join")
{
    TEST_CASE("Склейка пустых диапазонов — пустой диапазон")
    {
        std::vector<int> first;
        std::vector<int> second;
        auto ranges = burst::make_range_vector(first, second);

        auto joint_range = burst::join(ranges);

        CHECK(joint_range.empty());
    }

    TEST_CASE("Возвращает пустой диапазон, если на вход подан пустой диапазон диапазонов")
    {
        auto empty = std::vector<boost::iterator_range<std::vector<int>::iterator>>{};

        auto joint_range = burst::join(empty);

        CHECK(joint_range.empty());
    }

    TEST_CASE("Склейка одного диапазона — сам этот диапазон")
    {
        int array[] = {1, 2, 3, 4};
        auto ranges = burst::make_range_vector(array);

        auto joint_range = burst::join(ranges);

        CHECK(joint_range == array);
    }

    TEST_CASE("Накладывающиеся диапазоны не смешиваеются")
    {
        auto  first = burst::make_list({1, 2, 3});
        auto second = burst::make_list({2, 3, 4});
        auto ranges = burst::make_range_vector(first, second);

        auto joint_range = burst::join(ranges);

        auto expected_collection = {1, 2, 3, 2, 3, 4};
        CHECK(joint_range == expected_collection);
    }

    TEST_CASE("Склейка нескольких диапазонов создаёт новый диапазон, в котором исходные диапазоны "
        "помещены последовательно друг за другом в порядке их перечисления")
    {
        auto  first = {'h', 'e'};
        auto second = {'l', 'l'};
        auto  third = {'o', '!'};
        auto ranges = burst::make_range_vector(first, second, third);

        auto joint_range = burst::join(ranges);

        std::string expected_collection("hello!");
        CHECK(joint_range == expected_collection);
    }

    TEST_CASE("Результирующий диапазон ссылается на исходные диапазоны, так что изменение "
        "элементов результирующего диапазона влечёт изменение соответствующих элементов исходного")
    {
        auto first = burst::make_vector({100, 50});
        auto second = burst::make_vector({70, 30});
        auto ranges = burst::make_range_vector(first, second);

        auto joint_range = burst::join(ranges);
        boost::for_each(joint_range, [] (auto & x) { x /= 10; });

        CHECK(first[0] == 10);
        CHECK(first[1] == 5);
        CHECK(second[0] == 7);
        CHECK(second[1] == 3);
    }

    TEST_CASE("Диапазон склейки произвольного доступа имеет метод size()")
    {
        auto  first = burst::make_vector({1, 0});
        auto second = burst::make_vector({3, 2});
        auto ranges = burst::make_range_vector(first, second);

        auto joint_range = burst::join(ranges);

        CHECK(joint_range.size() == 4);
    }

    TEST_CASE("Диапазон склейки произвольного доступа может быть обращён")
    {
        std::string join("join");
        std::string iterator("iterator");
        std::string can("can");
        std::string be("be");
        std::string reversed("reversed");
        auto ranges = burst::make_range_vector(join, iterator, can, be, reversed);

        auto joint_range = burst::join(ranges);
        auto reversed_range = boost::adaptors::reverse(joint_range);

        std::string expected = join + iterator + can + be + reversed;
        std::reverse(expected.begin(), expected.end());
        CHECK(reversed_range == expected);
    }

    TEST_CASE("Однопроходный диапазон склейки пропускает пустой диапазон в начале")
    {
        const auto empty = burst::make_list<int>({});
        const auto first = burst::make_list({17, 19, 23});
        auto ranges = burst::make_range_vector(empty, first);

        const auto joint_range = burst::join(ranges);

        const auto expected_collection = {17, 19, 23};
        CHECK(joint_range == expected_collection);
    }

    TEST_CASE("Однопроходный диапазон склейки пропускает пустой диапазон в середине")
    {
        const auto first = burst::make_list({17, 19, 23});
        const auto empty = burst::make_list<int>({});
        const auto second = burst::make_list({29, 31, 37});
        auto ranges = burst::make_range_vector(first, empty, second);

        const auto joint_range = burst::join(ranges);

        const auto expected_collection = {17, 19, 23, 29, 31, 37};
        CHECK(joint_range == expected_collection);
    }

    TEST_CASE("Однопроходный диапазон склейки пропускает пустой диапазон в конце")
    {
        const auto first = burst::make_list({17, 19, 23});
        const auto empty = burst::make_list<int>({});
        auto ranges = burst::make_range_vector(first, empty);

        const auto joint_range = burst::join(ranges);

        const auto expected_collection = {17, 19, 23};
        CHECK(joint_range == expected_collection);
    }

    TEST_CASE("Диапазон склейки произвольного доступа пропускает пустой диапазон в начале")
    {
        const auto empty = burst::make_vector<int>({});
        const auto first = burst::make_vector({17, 19, 23});
        auto ranges = burst::make_range_vector(empty, first);

        const auto joint_range = burst::join(ranges);

        const auto expected_collection = {17, 19, 23};
        CHECK(joint_range == expected_collection);
    }

    TEST_CASE("Диапазон склейки произвольного доступа пропускает пустой диапазон в середине")
    {
        const auto first = burst::make_vector({17, 19, 23});
        const auto empty = burst::make_vector<int>({});
        const auto second = burst::make_vector({29, 31, 37});
        auto ranges = burst::make_range_vector(first, empty, second);

        const auto joint_range = burst::join(ranges);

        const auto expected_collection = {17, 19, 23, 29, 31, 37};
        CHECK(joint_range == expected_collection);
    }

    TEST_CASE("Диапазон склейки произвольного доступа пропускает пустой диапазон в конце")
    {
        const auto first = burst::make_vector({17, 19, 23});
        const auto empty = burst::make_vector<int>({});
        auto ranges = burst::make_range_vector(first, empty);

        const auto joint_range = burst::join(ranges);

        const auto expected_collection = {17, 19, 23};
        CHECK(joint_range == expected_collection);
    }

    TEST_CASE("Однопроходный диапазон склейки продвигает внутренние диапазоны до конца")
    {
        const auto one = burst::make_list({1, 2, 3});
        const auto another = burst::make_list({4, 5, 6});
        auto ranges = burst::make_range_vector(one, another);

        const auto empty = [] (const auto & range) { return range.empty(); };
        REQUIRE(boost::algorithm::none_of(ranges, empty));

        for (auto x: burst::join(ranges)) { static_cast<void>(x); };

        CHECK(boost::algorithm::all_of(ranges, empty));
    }
}
