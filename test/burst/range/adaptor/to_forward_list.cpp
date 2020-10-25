#include <burst/range/adaptor/to_forward_list.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>

#include <forward_list>
#include <initializer_list>
#include <list>

TEST_SUITE("to_forward_list")
{
    TEST_CASE("Может работать с rvalue-диапазоном")
    {
        const auto fl = boost::irange<int>(0, 10) | burst::to_forward_list;
        CHECK(fl == std::forward_list<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    }

    TEST_CASE("Может работать с lvalue-диапазоном")
    {
        const auto l = std::list<char>{'a', 'b', 'c'};

        const auto fl = l | burst::to_forward_list;
        CHECK(fl == std::forward_list<char>{'a', 'b', 'c'});
    }

    TEST_CASE("Может работать со списком инициализации")
    {
        auto il = {'a', 'b', 'c'};
        const auto fl = il | burst::to_forward_list;
        CHECK(fl == std::forward_list<char>{'a', 'b', 'c'});
    }
}
