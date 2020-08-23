#include <burst/container/make_vector.hpp>
#include <utility/counters.hpp>
#include <utility/io/deque.hpp>
#include <utility/io/forward_list.hpp>
#include <utility/io/list.hpp>
#include <utility/io/vector.hpp>

#include <doctest/doctest.h>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/range/irange.hpp>

#include <deque>
#include <forward_list>
#include <initializer_list>
#include <list>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace
{
    template <template <typename ...> class SequenceContainer>
    struct make_sequence_t
    {
        template <typename Value, typename ... Xs>
        static decltype(auto) apply (std::initializer_list<Value> values, Xs && ... xs)
        {
            return burst::make_sequence_container<SequenceContainer>(values, std::forward<Xs>(xs)...);
        }

        template <typename ... Xs>
        static decltype(auto) apply (Xs && ... xs)
        {
            return burst::make_sequence_container<SequenceContainer>(std::forward<Xs>(xs)...);
        }

        template <typename Value, typename ... Xs>
        static decltype(auto) apply (Xs && ... xs)
        {
            return burst::make_sequence_container<SequenceContainer, Value>(std::forward<Xs>(xs)...);
        }

        template <typename Value>
        static auto actual (std::initializer_list<Value> values)
        {
            return SequenceContainer<Value>(values);
        }
    };
}

#define SIZED_SEQUENCE_GENERATORS\
    make_sequence_t<std::deque>,\
    make_sequence_t<std::list>,\
    make_sequence_t<std::vector>

#define SEQUENCE_GENERATORS\
    SIZED_SEQUENCE_GENERATORS,\
    make_sequence_t<std::forward_list>

TEST_SUITE("make_sequence_container")
{
    TEST_CASE_TEMPLATE("Тип элементов создаваемого контейнера выводится из типа элементов входного "
        "диапазона", make_sequence, SEQUENCE_GENERATORS)
    {
        const auto range = boost::irange(0, 4);
        const auto v = make_sequence::apply(range);
        CHECK
        ((
            std::is_same
            <
                typename decltype(v)::value_type,
                boost::range_value<decltype(range)>::type
            >
            ::value
        ));
    }

    TEST_CASE_TEMPLATE("Принимает диапазон по rvalue",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto v = make_sequence::apply(boost::irange<std::uint32_t>(0, 6));
        CHECK(v == make_sequence::template actual<std::uint32_t>({0, 1, 2, 3, 4, 5}));
    }

    TEST_CASE_TEMPLATE("Принимает диапазон по lvalue",
        make_sequence, SEQUENCE_GENERATORS)
    {
        std::stringstream stream("5 4 3 2");
        const auto range =
            boost::make_iterator_range
            (
                std::istream_iterator<int>(stream),
                std::istream_iterator<int>{}
            );

        const auto v = make_sequence::apply(range);
        CHECK(v == make_sequence::template actual<int>({5, 4, 3, 2}));
    }

    TEST_CASE_TEMPLATE("Принимает контейнеры",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto initial = std::vector<int>{1, 2, 3};

        const auto v = make_sequence::apply(initial);
        CHECK(v == make_sequence::template actual<int>({1, 2, 3}));
    }

    TEST_CASE_TEMPLATE("Принимает контейнеры по rvalue",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto v = make_sequence::apply(std::vector<int>{1, 2, 3});
        CHECK(v == make_sequence::template actual<int>({1, 2, 3}));
    }

    TEST_CASE_TEMPLATE("Есть перегрузка для списка инициализации",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto v = make_sequence::apply({1, 2, 3});
        CHECK(v == make_sequence::template actual<int>({1, 2, 3}));
    }

    TEST_CASE_TEMPLATE("Есть перегрузка для списка инициализации с аллокатором",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto v = make_sequence::apply({4, 5, 6}, std::allocator<int>{});
        CHECK(v == make_sequence::template actual<int>({4, 5, 6}));
    }

    TEST_CASE_TEMPLATE("Есть перегрузка для диапазона с аллокатором",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto v =
            make_sequence::apply
            (
                boost::irange<std::uint32_t>(0, 6),
                std::allocator<std::uint32_t>{}
            );

        CHECK(v == make_sequence::template actual<std::uint32_t>({0, 1, 2, 3, 4, 5}));
    }

    TEST_CASE_TEMPLATE("Тип элементов может быть указан явно при конструировании из диапазона",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto v = make_sequence::template apply<std::size_t>(boost::irange<int>(0, 4));
        CHECK
        ((
            std::is_same
            <
                typename decltype(v)::value_type,
                std::size_t
            >
            ::value
        ));
    }

    TEST_CASE_TEMPLATE("Тип элементов может быть указан явно при конструировании из диапазона с "
        "аллокатором", make_sequence, SEQUENCE_GENERATORS)
    {
        const auto v =
            make_sequence::template apply<std::size_t>
            (
                boost::irange<int>(0, 4),
                std::allocator<std::size_t>{}
            );
        CHECK
        ((
            std::is_same
            <
                typename decltype(v)::value_type,
                std::size_t
            >
            ::value
        ));
    }

    TEST_CASE_TEMPLATE("Умеет инициализироваться N копиями заданного значения",
        make_sequence, SIZED_SEQUENCE_GENERATORS)
    {
        const auto v = make_sequence::apply(5u, std::string{"qwe"});

        CHECK(v.size() == 5);
        CHECK((std::is_same<typename decltype(v)::value_type, std::string>::value));
        CHECK(boost::algorithm::all_of(v, [] (const auto & s) {return s == "qwe";}));
    }

    TEST_CASE("Умеет инициализировать std::forward_list N копиями заданного значения")
    {
        const auto v = burst::make_sequence_container<std::forward_list>(5u, std::string{});

        CHECK(std::distance(v.begin(), v.end()) == 5);
        CHECK((std::is_same<decltype(v)::value_type, std::string>::value));
        CHECK(boost::algorithm::all_of(v, [] (const auto & s) {return s.empty();}));
    }

    TEST_CASE_TEMPLATE("Есть перегрузка для двух итераторов",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto l = {1, 2, 3};
        const auto v = make_sequence::apply(l.begin(), l.end());
        CHECK(v == make_sequence::template actual<int>({1, 2, 3}));
    }

    TEST_CASE_TEMPLATE("Есть перегрузка для двух итераторов с явно заданным типом значений",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto l = {1, 2, 3};
        const auto v = make_sequence::template apply<short>(l.begin(), l.end());
        CHECK
        ((
            std::is_same
            <
                typename decltype(v)::value_type,
                short
            >
            ::value
        ));
    }

    TEST_CASE_TEMPLATE("Есть перегрузка для двух итераторов с аллокатором",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto l = {1, 2, 3};
        const auto v = make_sequence::apply(l.begin(), l.end(), std::allocator<int>{});
        CHECK(v == make_sequence::template actual<int>({1, 2, 3}));
    }

    TEST_CASE_TEMPLATE("Есть перегрузка для двух итераторов с аллокатором и явно заданным типом "
        "значений", make_sequence, SEQUENCE_GENERATORS)
    {
        const auto l = {1, 2, 3};
        const auto v = make_sequence::template apply<double>(l.begin(), l.end(), std::allocator<double>{});
        CHECK
        ((
            std::is_same
            <
                typename decltype(v)::value_type,
                double
            >
            ::value
        ));
    }

    TEST_CASE_TEMPLATE("Пробрасывает rvalue-контейнер как ссылку на rvalue",
        make_sequence, SEQUENCE_GENERATORS)
    {
        using inner_type = decltype(make_sequence::actual({1, 2, 3}));
        using outer_type = decltype(make_sequence::apply(std::declval<inner_type&&>()));

        CHECK(std::is_same<outer_type, inner_type &&>::value);
    }

    TEST_CASE_TEMPLATE("При пробрасывании rvalue-контейнера не производит ни копирований, ни "
        "переносов", make_sequence, SEQUENCE_GENERATORS)
    {
        auto copy_count = std::size_t{0};
        auto move_count = std::size_t{0};

        const auto v =
            make_sequence::apply
            (
                // Создаём контейнер из только что созданного контейнера.
                make_sequence::apply
                ({
                    utility::copy_move_constructor_counter{copy_count, move_count}
                })
            );

        CHECK(not v.empty());
        // Только одно копирование при вызове конструктора изначального контейнера.
        CHECK(copy_count == 1);
        // Больше никаких копирований и переносов.
        CHECK(move_count == 0);
    }
}
