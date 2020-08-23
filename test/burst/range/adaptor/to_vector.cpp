#include <burst/range/adaptor/to_vector.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>

#include <forward_list>
#include <initializer_list>
#include <vector>

TEST_SUITE("to_vector")
{
    TEST_CASE("Может работать с rvalue-диапазоном")
    {
        const auto v = boost::irange<int>(0, 10) | burst::to_vector;
        CHECK(v == std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    }

    TEST_CASE("Может работать с lvalue-диапазоном")
    {
        const auto fl = std::forward_list<char>{'a', 'b', 'c'};

        const auto v = fl | burst::to_vector;
        CHECK(v == std::vector<char>{'a', 'b', 'c'});
    }

    TEST_CASE("Может работать со списком инициализации")
    {
        auto l = {'a', 'b', 'c'};
        const auto v = l | burst::to_vector;
        CHECK(v == std::vector<char>{'a', 'b', 'c'});
    }
}
