#include <burst/tuple/get.hpp>

#include <doctest/doctest.h>
#include <boost/tuple/tuple.hpp>

#include <tuple>
#include <type_traits>
#include <utility>

TEST_SUITE("get")
{
    TEST_CASE("Возвращает заданный элемент кортежа")
    {
        CHECK(burst::get<0>(std::make_tuple('a', 'b', 'c')) == 'a');
    }

    TEST_CASE("Является функциональным объектом")
    {
        auto g = burst::get<0>;
        CHECK(g(std::make_tuple('a', 'b', 'c')) == 'a');
    }

    TEST_CASE("Элемент lvalue-кортежа возвращается как lvalue")
    {
        auto t = std::make_tuple(42, "qwe", 3.14);

        burst::get<0>(t) = 17;

        CHECK(std::get<0>(t) == 17);
    }

    TEST_CASE("Элемент const lvalue-кортежа возвращается как const lvalue")
    {
        const auto t = std::make_tuple(42, "qwe", 3.14);

        using element_type = std::tuple_element<0, decltype(t)>::type;
        using result_type = decltype(burst::get<0>(t));

        CHECK((std::is_same<result_type, const element_type &>::value));
    }

    TEST_CASE("Элемент rvalue-кортежа возвращается как ссылка на rvalue")
    {
        CHECK
        (
            std::is_rvalue_reference
            <
                decltype(burst::get<0>(std::make_tuple(42, "qwe", 3.14)))
            >
            ::value
        );
    }

    TEST_CASE("Может быть вычислен на этапе компиляции")
    {
        constexpr auto r = burst::get<1>(std::make_tuple(0, 11, 222));
        CHECK(r == 11);
    }

    TEST_CASE("Совместим с std::pair")
    {
        const auto p = std::make_pair('c', 3.14);
        CHECK(burst::get<0>(p) == 'c');
    }

    TEST_CASE("Совместим с boost::tuple")
    {
        const auto t = boost::make_tuple(42, 3.14, true);
        CHECK(burst::get<2>(t) == true);
    }
}
