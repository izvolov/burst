#include <burst/container/access/front.hpp>
#include <burst/container/make_vector.hpp>

#include <doctest/doctest.h>

#include <memory>
#include <string>
#include <type_traits>

TEST_SUITE("front")
{
    TEST_CASE("Возвращает ссылку на элемент контейнера")
    {
        auto v = burst::make_vector({1, 2, 3});
        static_assert(std::is_same<decltype(burst::front(v)), int &>::value, "");
    }

    TEST_CASE("Возвращает неизменяемую ссылку на элемент неизменяемого контейнера")
    {
        const auto v = burst::make_vector({1, 2, 3});
        static_assert(std::is_same<decltype(burst::front(v)), const int &>::value, "");
    }

    TEST_CASE("Возвращает ни что иное, как первый элемент контейнера")
    {
        auto v = burst::make_vector({1, 2, 3});
        CHECK(std::addressof(burst::front(v)) == std::addressof(v.front()));

        burst::front(v) = 7;
        CHECK(v.front() == 7);
    }

    TEST_CASE("Возвращает ссылку на элемент массива")
    {
        std::string strings[] = {"123", "456", "789"};
        static_assert(std::is_same<decltype(burst::front(strings)), std::string &>::value, "");
    }

    TEST_CASE("Возвращает неизменяемую ссылку на элемент неизменяемого массива")
    {
        const std::string strings[] = {"123", "456", "789"};
        static_assert(std::is_same<decltype(burst::front(strings)), const std::string &>::value, "");
    }

    TEST_CASE("Адресует ни что иное, как первый элемент массива")
    {
        char array[] = {'a', 'b', 'c'};
        CHECK(std::addressof(burst::front(array)) == array);

        burst::front(array) = 'q';
        CHECK(array[0] == 'q');
    }

    TEST_CASE("Является функциональным объектом")
    {
        const auto v = burst::make_vector({1, 2, 3});
        const auto f = burst::front;

        CHECK(f(v) == 1);
    }
}
