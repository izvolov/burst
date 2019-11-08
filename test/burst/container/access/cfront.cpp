#include <burst/container/access/cfront.hpp>
#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>

#include <doctest/doctest.h>

#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>

TEST_SUITE("cfront")
{
    TEST_CASE("Всегда возвращает ссылку на неизменяемый элемент контейнера")
    {
        auto integers = burst::make_vector({1, 2, 3});
        static_assert(std::is_same<decltype(burst::cfront(integers)), const int &>::value, "");

        const auto characters = burst::make_vector({'a', 'b', 'c'});
        static_assert(std::is_same<decltype(burst::cfront(characters)), const char &>::value, "");
    }

    TEST_CASE("Возвращает ни что иное, как первый элемент контейнера")
    {
        auto reals = burst::make_list({1.6, 3.14, 2.7});
        CHECK(std::addressof(burst::cfront(reals)) == std::addressof(reals.front()));
    }

    TEST_CASE("Всегда возвращает ссылку на неизменяемый элемент массива")
    {
        std::string strings[] = {"123", "456", "789"};
        static_assert(std::is_same<decltype(burst::cfront(strings)), const std::string &>::value, "");

        const std::size_t integers[] = {12, 34, 56};
        static_assert(std::is_same<decltype(burst::cfront(integers)), const std::size_t &>::value, "");
    }

    TEST_CASE("Адресует ни что иное, как первый элемент массива")
    {
        int array[] = {1, 2, 3};
        CHECK(std::addressof(burst::cfront(array)) == array);
    }

    TEST_CASE("Является функциональным объектом")
    {
        const auto v = burst::make_vector({1, 2, 3});
        const auto cf = burst::cfront;

        CHECK(cf(v) == 1);
    }
}
