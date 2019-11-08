#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/own_as_range.hpp>

#include <doctest/doctest.h>

#include <algorithm>

TEST_SUITE("own_as_range")
{
    TEST_CASE("Итерирует контейнер, над которым установлено владение")
    {
        const auto owned = burst::own_as_range(burst::make_vector({1, 2, 3, 4}));

        const auto expected = {1, 2, 3, 4};
        CHECK(owned == expected);
    }

    TEST_CASE("Диапазон владеющих итераторов изменяем")
    {
        auto owned = burst::own_as_range(burst::make_forward_list({1, 2, 3, 4}));
        std::transform(owned.begin(), owned.end(), owned.begin(), [] (auto x) {return x * 10;});

        const auto expected = {10, 20, 30, 40};
        CHECK(owned == expected);
    }
}
