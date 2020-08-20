#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/integer/divceil.hpp>
#include <burst/range/buffered_chunks.hpp>
#include <burst/range/istream_range.hpp>

#include <doctest/doctest.h>

#include <boost/range/iterator_range.hpp>

#include <iterator>
#include <sstream>
#include <vector>

TEST_SUITE("buffered_chunks")
{
    TEST_CASE("Диапазон кусков пустого диапазона — пустой диапазон")
    {
        const auto v = std::vector<int>{};

        auto chunks = burst::buffered_chunks(v, 10);

        CHECK(chunks.empty());
    }

    TEST_CASE("Кусков столько, сколь нужно, чтобы разместить на них все элементы исходного "
        "диапазона")
    {
        const auto v = burst::make_vector(100u, 'q');
        const auto n = 13;

        auto chunks = burst::buffered_chunks(v, n);

        CHECK(std::distance(chunks.begin(), chunks.end()) == burst::divceil(v.size(), n));
    }

    TEST_CASE("Работает с однопроходным диапазоном")
    {
        auto s = std::stringstream("5 4 3 2 1");

        auto chunks = burst::buffered_chunks(burst::make_istream_range<int>(s), 3);

        const auto expected_first = {5, 4, 3};
        CHECK(chunks.front() == expected_first);

        chunks.advance_begin(1);
        const auto expected_second = {2, 1};
        CHECK(chunks.front() == expected_second);
    }

    TEST_CASE("Изменяемость кусков не зависит от изменяемости исходного диапазона")
    {
        const auto v = burst::make_list({'q', 'w', 'e', 'r'});
        const auto n = 2;

        auto chunks = burst::buffered_chunks(v, n);

        for (auto & x: chunks.front())
        {
            x = 17;
        }

        for (const auto & x: chunks.front())
        {
            CHECK(x == 17);
        }
    }
}
