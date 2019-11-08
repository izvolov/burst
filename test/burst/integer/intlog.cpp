#include <burst/integer/intlog.hpp>

#include <doctest/doctest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

TEST_SUITE("intlog")
{
    TEST_CASE("Порождает исключение, если логарифмируемое число находится вне области определения")
    {
        CHECK_THROWS_AS(burst::intlog(0ul, 10ul), std::domain_error);
    }

    TEST_CASE("Порождает исключение при неположительном основании")
    {
        CHECK_THROWS_AS(burst::intlog(10ul, 0ul), std::domain_error);
    }

    TEST_CASE("Порождает исключение при основании равном единице")
    {
        CHECK_THROWS_AS(burst::intlog(10ul, 1ul), std::domain_error);
    }

    TEST_CASE("Логарифм единицы по любому основанию равен нулю")
    {
        CHECK(burst::intlog(1u, 100500u) == 0);
    }

    TEST_CASE("Логарифм степени основания в точности равен этой степени")
    {
        CHECK(burst::intlog(100ul, 10ul) == 2);
    }

    TEST_CASE("Возвращает целую часть логарифма числа по заданному основанию")
    {
        CHECK(burst::intlog(30ul, 3ul) == 3);
    }

    TEST_CASE("Может быть вычислена на этапе компиляции")
    {
        constexpr auto x = burst::intlog(20ul, 2ul);
        CHECK(x == 4);
    }

    TEST_CASE("Допускает знаковые значения")
    {
        CHECK(burst::intlog(13, 3) == 2);
    }

    TEST_CASE("Допускает разные типы первого и второго аргументов")
    {
        constexpr auto int32_max = std::numeric_limits<std::int32_t>::max();
        CHECK(burst::intlog(std::int32_t{14}, std::uint64_t{int32_max} + 1) == 0);

        constexpr auto int8_max = std::numeric_limits<std::int8_t>::max();
        CHECK(burst::intlog(std::uint32_t{int8_max} + 1, std::int8_t{5}) == 3);
    }

    TEST_CASE("Тип возвращаемого значения равен типу первого аргумента")
    {
        using first_argument_type = std::uint32_t;
        using second_argument_type = std::int64_t;
        static_assert(not std::is_same<first_argument_type, second_argument_type>::value, "");

        using result_type = decltype(burst::intlog(first_argument_type{2}, second_argument_type{5}));

        CHECK(std::is_same<result_type, first_argument_type>::value);
    }
}
