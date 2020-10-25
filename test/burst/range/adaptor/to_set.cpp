#include <burst/range/adaptor/to_set.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>

#include <forward_list>
#include <functional>
#include <initializer_list>
#include <set>

TEST_SUITE("to_set")
{
    TEST_CASE("Может работать с rvalue-диапазоном")
    {
        const auto s = boost::irange<int>(0, 10) | burst::to_set;
        CHECK(s == std::set<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    }

    TEST_CASE("Может работать с lvalue-диапазоном")
    {
        const auto fl = std::forward_list<char>{'a', 'b', 'c'};

        const auto s = fl | burst::to_set;
        CHECK(s == std::set<char>{'a', 'b', 'c'});
    }

    TEST_CASE("Может работать со списком инициализации")
    {
        auto il = {'a', 'b', 'c'};
        const auto s = il | burst::to_set;
        CHECK(s == std::set<char>{'a', 'b', 'c'});
    }

    TEST_CASE("Предусматривает задание отношения порядка")
    {
        const auto fl = std::forward_list<char>{1, 2, 3};

        const auto s = fl | burst::to_set(std::greater<>{});
        CHECK(s == std::set<char, std::greater<>>{3, 2, 1});
    }
}
