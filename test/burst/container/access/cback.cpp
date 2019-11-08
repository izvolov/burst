#include <burst/container/access/cback.hpp>
#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>

#include <doctest/doctest.h>

#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>

TEST_SUITE("cback")
{
    TEST_CASE("Всегда возвращает ссылку на неизменяемый элемент контейнера")
    {
        auto integers = burst::make_vector({1, 2, 3});
        static_assert(std::is_same<decltype(burst::cback(integers)), const int &>::value, "");

        const auto characters = burst::make_vector({'a', 'b', 'c'});
        static_assert(std::is_same<decltype(burst::cback(characters)), const char &>::value, "");
    }

    TEST_CASE("Возвращает ни что иное, как последний элемент контейнера")
    {
        auto reals = burst::make_list({1.6, 3.14, 2.7});
        CHECK(std::addressof(burst::cback(reals)) == std::addressof(reals.back()));
    }

    TEST_CASE("Всегда возвращает ссылку на неизменяемый элемент массива")
    {
        std::string strings[] = {"123", "456", "789"};
        static_assert(std::is_same<decltype(burst::cback(strings)), const std::string &>::value, "");

        const std::size_t integers[] = {12, 34, 56};
        static_assert(std::is_same<decltype(burst::cback(integers)), const std::size_t &>::value, "");
    }

    TEST_CASE("Адресует ни что иное, как последний элемент массива")
    {
        int array[] = {1, 2, 3};
        CHECK(std::addressof(burst::cback(array)) == array + 2);
    }

    TEST_CASE("Является функциональным объектом")
    {
        const auto v = burst::make_vector({1, 2, 3});
        const auto cb = burst::cback;

        CHECK(cb(v) == 3);
    }
}
