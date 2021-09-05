#include <burst/algorithm/partial_sum_max.hpp>
#include <burst/container/access/front.hpp>
#include <burst/container/make_vector.hpp>

#include <doctest/doctest.h>

#include <boost/range/adaptor/transformed.hpp>

#include <iterator>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

TEST_SUITE("partial_sum_max")
{
    TEST_CASE("Возвращает максимальное значение входного диапазона и итератор за последним "
        "элементом частичной суммы в выходном диапазоне")
    {
        auto items = burst::make_vector({1, 4, -5, 7, 0});
        //                                         ^max

        auto partial_sums = std::vector<int>(items.size() * 2);
        const auto result = burst::partial_sum_max(items.begin(), items.end(), partial_sums.begin());

        const auto size = distance(items.begin(), items.end());
        CHECK(result == std::make_pair(partial_sums.begin() + size, 7));
    }

    TEST_CASE("Выходной диапазон может перекрывать входной")
    {
        auto items = burst::make_vector({5, -10, 4, -25, 3, 20, 2, 1, 0});

        const auto result = burst::partial_sum_max(items.begin(), items.end(), items.begin());

        const auto max = result.second;
        CHECK(max == 20);

        REQUIRE(result.first == items.end());
        const auto expected = burst::make_vector({5, -5, -1, -26, -23, -3, -1, 0, 0});
        CHECK(items == expected);
    }

    TEST_CASE("Для пустого диапазона возвращает итератор на начало, значение максимума при этом "
        "не оговаривается")
    {
        auto items = std::vector<int>{};

        const auto result = burst::partial_sum_max(items, items.begin());

        CHECK(result.first == items.begin());
        CHECK(result.first == items.end());
    }

    TEST_CASE("В результате работы алгоритма выходной диапазон заполнен частичными суммами "
        "входного диапазона")
    {
        auto items = burst::make_vector({2, 1, 3, 1, 4, 1, 5});

        auto partial_sums = std::vector<int>{};
        burst::partial_sum_max(items.begin(), items.end(), std::back_inserter(partial_sums));

        const auto expected = burst::make_vector({2, 3, 6, 7, 11, 12, 17});
        CHECK(partial_sums == expected);
    }

    TEST_CASE("Работает с однопроходными диапазонами")
    {
        std::string string_range("1 2 3 4 5");
        std::istringstream is(string_range);

        std::ostringstream os;

        auto partial_sums = std::vector<int>{};
        const auto result =
            burst::partial_sum_max
            (
                std::istream_iterator<int>(is),
                std::istream_iterator<int>{},
                std::ostream_iterator<int>(os, ",")
            );

        const auto max = result.second;
        CHECK(max == 5);

        const auto expected = std::string("1,3,6,10,15,");
        CHECK(os.str() == expected);
    }

    TEST_CASE("Можно задавать функцию суммирования, при этом на поиск максимума это не влияет")
    {
        //                                           max
        //                                          v
        auto items = burst::make_vector({10, 6, 11, 12, 4, 7, 2, 1, 6});
        //                               ----v
        //                                   -----------v
        //                                              ------v
        //                                                    ---v
        //                                                       ----

        auto partial_sums = std::vector<int>{};
        const auto result =
            burst::partial_sum_max(items.begin(), items.end(), std::back_inserter(partial_sums),
                [] (auto x, auto y)
                {
                    return std::min(x, y);
                });

        const auto max = result.second;
        CHECK(max == 12);

        const auto expected = burst::make_vector({10, 6, 6, 6, 4, 4, 2, 1, 1});
        CHECK(partial_sums == expected);
    }

    TEST_CASE("Можно задавать предикат для поиска максимума")
    {
        auto items = std::vector<std::string>{"1", "123", "00000", "356", "-10", ""};
        //                                                ^ самая длинная

        auto partial_sums = std::vector<std::string>(items.size());
        const auto result =
            burst::partial_sum_max(items, partial_sums.begin(), std::plus<>{},
                [] (const auto & l, const auto & r)
                {
                    return l.size() < r.size();
                });

        REQUIRE(partial_sums.back() == std::string("112300000356-10"));

        const auto & max = result.second;
        CHECK(max == std::string("00000"));
    }

    TEST_CASE("Умеет принимать move_iterator")
    {
        auto items = std::vector<std::vector<int>>{};
        items.push_back(std::vector<int>{1});
        items.push_back(std::vector<int>{2});
        items.push_back(std::vector<int>{3});
        items.push_back(std::vector<int>{2});
        items.push_back(std::vector<int>{1});

        auto partial_sums = std::vector<std::vector<int>>(items.size());
        const auto max =
            burst::partial_sum_max
            (
                std::make_move_iterator(items.begin()),
                std::make_move_iterator(items.end()),
                partial_sums.begin(),
                [] (auto && sum, const auto & next)
                {
                    return std::vector<int>{sum.front() + next.front()};
                },
                [] (const auto & l, const auto & r)
                {
                    return l.front() < r.front();
                }
            );

        const auto expected = {1, 3, 6, 8, 9};
        CHECK((partial_sums | boost::adaptors::transformed(burst::front)) == expected);
        CHECK(max.second.front() == 3);
    }
}
