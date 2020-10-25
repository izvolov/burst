#include <burst/range/adaptor/to_list.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>

#include <forward_list>
#include <initializer_list>
#include <list>

TEST_SUITE("to_list")
{
    TEST_CASE("Может работать с rvalue-диапазоном")
    {
        const auto l = boost::irange<int>(0, 10) | burst::to_list;
        CHECK(l == std::list<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    }

    TEST_CASE("Может работать с lvalue-диапазоном")
    {
        const auto fl = std::forward_list<char>{'a', 'b', 'c'};

        const auto l = fl | burst::to_list;
        CHECK(l == std::list<char>{'a', 'b', 'c'});
    }

    TEST_CASE("Может работать со списком инициализации")
    {
        auto il = {'a', 'b', 'c'};
        const auto l = il | burst::to_list;
        CHECK(l == std::list<char>{'a', 'b', 'c'});
    }
}
