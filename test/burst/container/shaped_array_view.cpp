#include <utility/counters.hpp>

#include <burst/container/shaped_array_view.hpp>

#include <doctest/doctest.h>

#include <cstddef>
#include <vector>

TEST_SUITE("shaped_array_view")
{
    TEST_CASE("Представляет одномерный массив в виде многомерного массива заданной формы")
    {
        int a[] = {1, 2, 3, 4, 5, 6};

        auto view = burst::make_shaped_array_view(a, burst::make_shape(2, 3));
        // 1 2 3 4 5
        // 6 7 8 9 10

        CHECK(view[0][0] == a[0]);
        CHECK(view[0][1] == a[1]);
        CHECK(view[0][2] == a[2]);
        CHECK(view[1][0] == a[3]);
        CHECK(view[1][1] == a[4]);
        CHECK(view[1][2] == a[5]);
    }

    TEST_CASE("Изменения в представлении отражаются на исходнике")
    {
        int a[8] = {0};

        auto view = burst::make_shaped_array_view(a, burst::make_shape(4, 2));
        // a
        // 0 0 0 0 0 0 0 0
        // view
        // 0 0
        // 0 0
        // 0 0
        // 0 0

        view[0][0] = 1;
        view[0][1] = 2;
        view[1][0] = 3;
        view[1][1] = 4;
        view[2][0] = 5;
        view[2][1] = 6;
        view[3][0] = 7;
        view[3][1] = 8;
        // view
        // 1 2
        // 3 4
        // 5 6
        // 7 8
        // a
        // 1 2 3 4 5 6 7 8

        for (auto i = 0; i < 8; ++i)
        {
            CHECK(a[i] == i + 1);
        }
    }

    TEST_CASE("Создание представления не приводит к созданию новых экземпляров элементов")
    {
        auto copy_count = std::size_t{0};
        auto move_count = std::size_t{0};
        auto counter = utility::copy_move_constructor_counter(copy_count, move_count);
        const auto v = std::vector<utility::copy_move_constructor_counter>(10, counter);

        auto old_copy_count = copy_count;
        auto old_move_count = move_count;
        auto view = burst::make_shaped_array_view(v.data(), burst::make_shape(5, 2));
        static_cast<void>(view);
        CHECK(copy_count == old_copy_count);
        CHECK(move_count == old_move_count);
    }

    TEST_CASE("Копирование представления не приводит к созданию новых экземпляров элементов")
    {
        auto copy_count = std::size_t{0};
        auto move_count = std::size_t{0};
        auto counter = utility::copy_move_constructor_counter(copy_count, move_count);
        const auto v = std::vector<utility::copy_move_constructor_counter>(10, counter);

        auto view = burst::make_shaped_array_view(v.data(), burst::make_shape(5, 2));
        auto old_copy_count = copy_count;
        auto old_move_count = move_count;
        auto view1 = view;
        static_cast<void>(view1);
        CHECK(copy_count == old_copy_count);
        CHECK(move_count == old_move_count);
    }

    TEST_CASE("Срез по представлению не приводит к созданию новых экземпляров элементов")
    {
        auto copy_count = std::size_t{0};
        auto move_count = std::size_t{0};
        auto counter = utility::copy_move_constructor_counter(copy_count, move_count);
        const auto v = std::vector<utility::copy_move_constructor_counter>(10, counter);

        auto view = burst::make_shaped_array_view(v.data(), burst::make_shape(5, 2));
        auto old_copy_count = copy_count;
        auto old_move_count = move_count;
        auto view1 = view[1];
        static_cast<void>(view1);
        CHECK(copy_count == old_copy_count);
        CHECK(move_count == old_move_count);
    }
}
