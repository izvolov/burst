#include <burst/integer/to_unsigned.hpp>

#include <doctest/doctest.h>

#include <boost/mpl/vector.hpp>

#include <limits>
#include <type_traits>

TEST_SUITE("to_unsigned")
{
    TEST_CASE_TEMPLATE("shifts_domain_of_a_signed_integral_to_unsigned", signed_integer_type,
        char, signed char, short, int, long, long long)
    {
        using unsigned_integer_type = std::make_unsigned_t<signed_integer_type>;
        CHECK
        (
            burst::to_unsigned(std::numeric_limits<signed_integer_type>::min()) ==
            std::numeric_limits<unsigned_integer_type>::min()
        );
        CHECK
        (
            burst::to_unsigned(std::numeric_limits<signed_integer_type>::max()) ==
            std::numeric_limits<unsigned_integer_type>::max()
        );
        CHECK
        (
            burst::to_unsigned(signed_integer_type{0}) ==
            std::numeric_limits<unsigned_integer_type>::max() / 2 + 1
        );
    }

    TEST_CASE_TEMPLATE("unsigned_values_remain_untouched", unsigned_integer_type,
        unsigned char, unsigned short, unsigned, unsigned long, unsigned long long)
    {
        CHECK
        (
            burst::to_unsigned(std::numeric_limits<unsigned_integer_type>::min()) ==
            std::numeric_limits<unsigned_integer_type>::min()
        );
        CHECK
        (
            burst::to_unsigned(std::numeric_limits<unsigned_integer_type>::max()) ==
            std::numeric_limits<unsigned_integer_type>::max()
        );
        CHECK
        (
            burst::to_unsigned(unsigned_integer_type{0}) ==
            unsigned_integer_type{0}
        );
        CHECK
        (
            burst::to_unsigned(unsigned_integer_type{1}) ==
            unsigned_integer_type{1}
        );
    }

    TEST_CASE("is_a_constexpr_function")
    {
        constexpr auto r = burst::to_unsigned(-1);
        static_assert(r == std::numeric_limits<unsigned>::max () / 2, "");
    }
}
