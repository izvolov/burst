#include <burst/container/access/back.hpp>
#include <burst/container/make_vector.hpp>

#include <doctest/doctest.h>

#include <memory>
#include <string>
#include <type_traits>

TEST_SUITE("back")
{
    TEST_CASE("Возвращает ссылку на элемент контейнера")
    {
        auto v = burst::make_vector({1, 2, 3});
        static_assert(std::is_same<decltype(burst::back(v)), int &>::value, "");
    }

    TEST_CASE("Возвращает неизменяемую ссылку на элемент неизменяемого контейнера")
    {
        const auto v = burst::make_vector({1, 2, 3});
        static_assert(std::is_same<decltype(burst::back(v)), const int &>::value, "");
    }

    TEST_CASE("Возвращает ни что иное, как последний элемент контейнера")
    {
        auto v = burst::make_vector({1, 2, 3});
        CHECK(std::addressof(burst::back(v)) == std::addressof(v.back()));

        burst::back(v) = 7;
        CHECK(v.back() == 7);
    }

    TEST_CASE("Возвращает ссылку на элемент массива")
    {
        std::string strings[] = {"123", "456", "789"};
        static_assert(std::is_same<decltype(burst::back(strings)), std::string &>::value, "");
    }

    TEST_CASE("Возвращает неизменяемую ссылку на элемент неизменяемого массива")
    {
        const std::string strings[] = {"123", "456", "789"};
        static_assert(std::is_same<decltype(burst::back(strings)), const std::string &>::value, "");
    }

    TEST_CASE("Возвращает ни что иное, как последний элемент массива")
    {
        char array[] = {'a', 'b', 'c'};
        CHECK(std::addressof(burst::back(array)) == array + 2);

        burst::back(array) = 'q';
        CHECK(array[2] == 'q');
    }

    TEST_CASE("Является функциональным объектом")
    {
        const auto v = burst::make_vector({1, 2, 3});
        const auto b = burst::back;

        CHECK(b(v) == 3);
    }
}
