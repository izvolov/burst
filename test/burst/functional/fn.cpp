#include <burst/functional/fn.hpp>

#include <doctest/doctest.h>

#include <cstdlib>
#include <type_traits>

namespace // anonymous
{
    struct dummy
    {
        int member;
    };

    const int & free_function (const dummy & d)
    {
        return d.member;
    }

    int & free_function (dummy & d)
    {
        return d.member;
    }

    int free_function (const dummy & d, int y)
    {
        return d.member + y;
    }
} // namespace anonymous

TEST_SUITE("fn_macro")
{
    TEST_CASE("Создаёт функциональный объект, вызывающий свободную функцию с заданным именем "
        "от экземпляра произвольного класса")
    {
        auto call_fn = BURST_FN(free_function);
        CHECK(call_fn(dummy{17}) == 17);
    }

    TEST_CASE("Возвращает ссылку на константу, если объект константный")
    {
        const auto fn = BURST_FN(free_function);
        const auto d = dummy{42};

        decltype(auto) m = fn(d);

        CHECK(std::is_same<decltype(m), const int &>::value);
    }

    TEST_CASE("Возвращает ссылку на изменяемое значение, если объект неконстантный")
    {
        const auto fn = BURST_FN(free_function);
        auto d = dummy{42};

        fn(d) = 17;

        CHECK(free_function(d) == 17);
    }

    TEST_CASE("Допускает перегрузки")
    {
        const auto fn = BURST_FN(free_function);
        const auto d = dummy{3};
        CHECK(fn(d, 4) == 7);
    }

    TEST_CASE("Компонуем")
    {
        const auto square_of_abs = BURST_FN(std::abs) | [] (auto x) {return x * x;};
        CHECK(square_of_abs(-5) == 25);
    }
}
