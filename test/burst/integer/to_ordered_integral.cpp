#include <burst/integer/to_ordered_integral.hpp>
#include <burst/type_traits/invoke_result.hpp>
#include <burst/type_traits/unsigned_integer_of_size.hpp>

#include <doctest/doctest.h>

#include <boost/mpl/vector.hpp>

#include <array>
#include <cstdint>
#include <limits>
#include <type_traits>

TEST_SUITE("to_ordered_integral")
{
    TEST_CASE_TEMPLATE("Отношение порядка на образах чисел с плавающей запятой соответствует "
        "отношению порядка на самих числах", floating_type, float, double)
    {
        const auto check =
            [] (auto x, auto y)
            {
                REQUIRE(x < y);
                CHECK(burst::to_ordered_integral(x) < burst::to_ordered_integral(y));
            };

        check(static_cast<floating_type>(0.1), static_cast<floating_type>(0.11));
        check(static_cast<floating_type>(0.00001), static_cast<floating_type>(0.00002));
        check(static_cast<floating_type>(-0.11), static_cast<floating_type>(-0.1));
        check(static_cast<floating_type>(-0.101), static_cast<floating_type>(-0.1));
        check(static_cast<floating_type>(-3.14), static_cast<floating_type>(3.14));
        check(static_cast<floating_type>(-0.0), static_cast<floating_type>(3.14));

        constexpr auto pos_inf = std::numeric_limits<floating_type>::infinity();
        constexpr auto neg_inf = static_cast<floating_type>(-pos_inf);
        constexpr auto min = std::numeric_limits<floating_type>::min();
        constexpr auto max = std::numeric_limits<floating_type>::max();
        check(neg_inf, min);
        check(neg_inf, static_cast<floating_type>(-3.14));
        check(neg_inf, static_cast<floating_type>(-0.0));
        check(neg_inf, static_cast<floating_type>(0.0));
        check(neg_inf, static_cast<floating_type>(3.14));
        check(neg_inf, max);
        check(neg_inf, pos_inf);
        check(min, pos_inf);
        check(static_cast<floating_type>(-2.71), pos_inf);
        check(static_cast<floating_type>(-0.0), pos_inf);
        check(static_cast<floating_type>(0.0), pos_inf);
        check(static_cast<floating_type>(3e-5), pos_inf);
        check(max, pos_inf);
    }

    TEST_CASE_TEMPLATE("Для целых чисел тип результата равен исходному типу", integral_type,
        std::int8_t, std::uint8_t,
        std::int16_t, std::uint16_t,
        std::int32_t, std::uint32_t,
        std::int64_t, std::uint64_t)
    {
        CHECK(std::is_same
        <
            burst::invoke_result_t<burst::to_ordered_integral_fn, integral_type>,
            integral_type
        >
        ::value);
    }

    TEST_CASE_TEMPLATE("Для типов с плавающей запятой и указателей тип результата соответствует "
        "целому числу такого же размера, как исходный тип", scalar_type, float, double)
    {
        CHECK(std::is_same
        <
            burst::invoke_result_t<burst::to_ordered_integral_fn, scalar_type>,
            burst::unsigned_integer_of_size_t<sizeof(scalar_type)>
        >
        ::value);
    }
}
