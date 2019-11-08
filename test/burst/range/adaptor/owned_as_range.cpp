#include <burst/container/make_forward_list.hpp>
#include <burst/range/adaptor/owned_as_range.hpp>
#include <utility/io/initializer_list.hpp>

#include <doctest/doctest.h>

TEST_SUITE("owned_as_range")
{
    TEST_CASE("Делает ровно то же самое, что и функция burst::own_as_range")
    {
        const auto owned_as_range = burst::make_forward_list({'a', 'b', 'c'}) | burst::owned_as_range;

        const auto expected = {'a', 'b', 'c'};
        CHECK(owned_as_range == expected);
    }
}
