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

    TEST_CASE_TEMPLATE("Отношение порядка на образах указателей соответствует отношению порядка "
        "на самих указателях", pointee_type, int, const double)
    {
        const auto a = std::array<pointee_type, 10>{};
        const auto first = a.data();
        const auto last = first + a.size();

        CHECK(burst::to_ordered_integral(first) < burst::to_ordered_integral(first + 1));
        CHECK(burst::to_ordered_integral(first) < burst::to_ordered_integral(first + a.size() / 2));
        CHECK(burst::to_ordered_integral(first) < burst::to_ordered_integral(last));
        CHECK(burst::to_ordered_integral(first + 1) < burst::to_ordered_integral(last));
        CHECK(burst::to_ordered_integral(first + a.size() / 2) < burst::to_ordered_integral(last));
    }

    TEST_CASE("Отношение порядка на образах перечислений соответствует отношению порядка "
        "на самих перечислениях")
    {
        enum struct e
        {
            x = 2,
            y = 1,
            z = 0,
        };

        CHECK(burst::to_ordered_integral(e::z) < burst::to_ordered_integral(e::y));
        CHECK(burst::to_ordered_integral(e::z) < burst::to_ordered_integral(e::x));
        CHECK(burst::to_ordered_integral(e::y) < burst::to_ordered_integral(e::x));
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
        "целому числу такого же размера, как исходный тип", scalar_type,
        float, double, int *, const char *)
    {
        CHECK(std::is_same
        <
            burst::invoke_result_t<burst::to_ordered_integral_fn, scalar_type>,
            burst::unsigned_integer_of_size_t<sizeof(scalar_type)>
        >
        ::value);
    }

    TEST_CASE_TEMPLATE("Для перечислений тип результата равен типу базового для перечисления целого", base_type,
        std::int8_t, std::uint8_t,
        std::int16_t, std::uint16_t,
        std::int32_t, std::uint32_t,
        std::int64_t, std::uint64_t)
    {
        SUBCASE("в случае обыкновенного перечисления")
        {
            enum simple_enum: base_type
            {
                x,
                y
            };

            CHECK(std::is_same
            <
                burst::invoke_result_t<burst::to_ordered_integral_fn, simple_enum>,
                std::underlying_type_t<simple_enum>
            >
            ::value);
        }

        SUBCASE("в случае enum struct")
        {
            enum struct enum_struct: base_type
            {
                x,
                y
            };

            CHECK(std::is_same
            <
                burst::invoke_result_t<burst::to_ordered_integral_fn, enum_struct>,
                std::underlying_type_t<enum_struct>
            >
            ::value);
        }

        SUBCASE("в случае enum class")
        {
            enum class enum_class: base_type
            {
                x,
                y
            };

            CHECK(std::is_same
            <
                burst::invoke_result_t<burst::to_ordered_integral_fn, enum_class>,
                std::underlying_type_t<enum_class>
            >
            ::value);
        }
    }
}
