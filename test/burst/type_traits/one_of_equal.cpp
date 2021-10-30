#include <burst/type_traits/one_of_equal.hpp>
#include <burst/type_traits/void_t.hpp>

#include <doctest/doctest.h>

#include <type_traits>

template <typename, typename = void>
struct substitution_failure: std::true_type {};

template <typename T>
struct substitution_failure<T, burst::void_t<typename T::type>>: std::false_type {};

TEST_SUITE("one_of_equal")
{
    TEST_CASE("Возвращает один (первый) из списка одинаковых типов")
    {
        CHECK(std::is_same<burst::one_of_equal_t<int, int>, int>::value);
        CHECK(std::is_same<burst::one_of_equal_t<char, char, char>, char>::value);
        CHECK(std::is_same<burst::one_of_equal_t<double>, double>::value);
    }

    TEST_CASE("Отваливается по SFINAE, если типы разные")
    {
        CHECK(substitution_failure<burst::one_of_equal<int, int, long>>::value);
        CHECK(substitution_failure<burst::one_of_equal<char, char, char, signed char>>::value);
        CHECK(substitution_failure<burst::one_of_equal<int, double, long>>::value);
    }
}
