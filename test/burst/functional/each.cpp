#include <burst/algorithm/sum.hpp>
#include <burst/functional/each.hpp>
#include <burst/functional/only.hpp>
#include <burst/integer/intlog2.hpp>
#include <burst/tuple/make_tuple.hpp>
#include <utility/caller_dummies.hpp>
#include <utility/io/tuple.hpp>

#include <doctest/doctest.h>

#include <cstddef>
#include <functional>
#include <string>
#include <tuple>

namespace // anonymous
{
    struct doubler
    {
        explicit doubler (std::size_t & calls):
            calls(calls)
        {
        }

        template <typename T>
        auto operator () (T t) const
        {
            ++calls;
            return t * t;
        }

        std::size_t & calls;
    };

    struct dummy
    {
        dummy ()
        {
            ++instances_count;
        }

        dummy (const dummy &)
        {
            ++instances_count;
        }

        dummy (dummy &&)
        {
            ++instances_count;
        }

        ~dummy ()
        {
            --instances_count;
        }

        static int instances_count;
    };

    int dummy::instances_count = 0;
} // namespace anonymous

TEST_SUITE("each")
{
    TEST_CASE("Пробрасывает входные аргументы, преобразуя их с помощью заданной функции")
    {
        auto e = burst::each([] (auto x) {return x + x;}) | burst::make_tuple;
        auto t = e(4, std::string("qwe"));
        CHECK(t == std::make_tuple(8, "qweqwe"));
    }

    TEST_CASE("Может быть скомпонована с другим burst::each")
    {
        const auto square = [] (auto x) {return x * x;};
        auto e = burst::each(square) | burst::each(square) | burst::sum;
        auto r = e(1, 2, 3);
        CHECK(r == 1 + 16 + 81);
    }

    TEST_CASE("Результат композиции может участвовать в новой композиции")
    {
        const auto square = [] (auto x) {return x * x;};
        auto e = burst::each(square) | burst::sum | square;
        auto r = e(1, 2, 3);
        CHECK(r == (1 + 4 + 9) * (1 + 4 + 9));
    }

    TEST_CASE("Вызывает хранимую функцию на каждый входной элемент")
    {
        auto calls = std::size_t{0};
        auto e = burst::each(doubler{calls}) | burst::make_tuple;

        e(4, 'a', 3.14);

        CHECK(calls == 3);
    }

    TEST_CASE("Каждая переданная функция хранится внутри созданного функционального объекта")
    {
        const auto old_instances_count = dummy::instances_count;

        auto e = burst::each(dummy{}) | burst::each(dummy{});
        static_cast<void>(e);

        CHECK(dummy::instances_count == old_instances_count + 2);
    }

    TEST_CASE("Функции, переданные с помощью std::ref, не хранятся внутри")
    {
        auto d = dummy{};
        const auto old_instances_count = dummy::instances_count;

        auto e = burst::each(std::ref(d));
        static_cast<void>(e);

        CHECK(dummy::instances_count == old_instances_count);
    }

    TEST_CASE("Если созданный функциональный объект вызывается как const lvalue, то хранимый "
        "функциональный объект вызывается так же")
    {
        auto calls = std::size_t{0};
        const auto each =
            burst::each(utility::const_lvalue_call_counter(calls)) | burst::make_tuple;

        each(1, "qwe");

        CHECK(calls == 2);
    }

    TEST_CASE("Если созданный функциональный объект вызывается как lvalue, то хранимый "
        "функциональный объект вызывается так же")
    {
        auto calls = std::size_t{0};
        auto each = burst::each(utility::lvalue_call_counter(calls)) | burst::make_tuple;

        each(1, "qwe");

        CHECK(calls == 2);
    }

    TEST_CASE("Если созданный функциональный объект вызывается как rvalue, то скомпонованный "
        "функциональный объект вызывается как rvalue")
    {
        auto calls = std::size_t{0};
        (burst::each([] (auto x) {return x + x;}) | utility::rvalue_call_counter(calls))(1, 2, 3);

        CHECK(calls == 1);
    }

    TEST_CASE("Если созданный функциональный объект вызывается как rvalue, то хранимый "
        "внутри each функциональный объект вызывается как lvalue")
    {
        auto calls = std::size_t{0};
        (burst::each(utility::lvalue_call_counter(calls)) | burst::make_tuple)(1, "qwe");

        CHECK(calls == 2);
    }

    TEST_CASE("Функции, переданные с помощью std::ref, всегда вызываются как lvalue")
    {
        auto calls = std::size_t{0};
        auto c = utility::lvalue_call_counter(calls);
        const auto each = burst::each(std::ref(c)) | burst::make_tuple;

        each(1, "qwe");

        CHECK(calls == 2);
    }

    TEST_CASE("Неизменяемые функции, переданные с помощью std::ref, всегда вызываются как "
        "const lvalue")
    {
        auto calls = std::size_t{0};
        const auto c = utility::const_lvalue_call_counter(calls);
        const auto each = burst::each(std::ref(c)) | burst::make_tuple;

        each(1, "qwe");

        CHECK(calls == 2);
    }

    TEST_CASE("Может быть вычислена на этапе компиляции")
    {
        constexpr auto e = burst::each(&burst::intlog2<int>) | burst::make_tuple;
        constexpr auto t = e(1, 256, 1024);
        CHECK(t == std::make_tuple(0, 8, 10));
    }

    TEST_CASE("Компонуема с burst::only")
    {
        const auto cube = [] (auto x) {return x * x * x;};
        const auto stringify = [] (auto x) {return std::to_string(x);};
        const auto twice = [] (auto x) {return x + x;};
        const auto size = [] (const auto & s) {return s.size();};

        const auto f =
            burst::only<1>(cube) |
            burst::each(stringify) |
            burst::only<0>(twice) |
            burst::each(size) |
            burst::sum;

        CHECK(f(10, 20, 30) == 10);
    }
}
