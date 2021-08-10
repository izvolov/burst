#ifndef BURST_CONTAINER_SHAPED_ARRAY_VIEW_HPP
#define BURST_CONTAINER_SHAPED_ARRAY_VIEW_HPP

#include <burst/container/detail/array_operations.hpp>
#include <burst/container/detail/shaped_array_view_base.hpp>
#include <burst/variadic.hpp>

#include <array>
#include <cassert>
#include <cstddef>
#include <functional>
#include <numeric>

namespace burst
{
    /*!
        \~russian
            \brief
                Многомерное представление одномерного массива

            \details
                Позволяет работать с линейным куском памяти так, как будто это многомерный массив,
                то есть массив в массиве в массиве... и т.д.

                Пример использования:

                \code{.cpp}
                int a[12] = {0};
                // a =
                //  [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

                auto view = burst::make_shaped_array_view(a, burst::make_shape(3, 4));
                // Создано двумерное представление, состоящее из трёх "строк" по четыре элемента.
                // view =
                //  [0, 0, 0, 0]
                //  [0, 0, 0, 0]
                //  [0, 0, 0, 0]

                view[1][2] = 7;
                // view =
                //  [0, 0, 0, 0]
                //  [0, 0, 7, 0]
                //  [0, 0, 0, 0]
                // a =
                //  [0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0]

                assert(a[4 * 1 + 2] == 7);
                \endcode

                По сути, это легковесный вариант boost::multi_array_view. От оригинала отличается
                только легковесностью и простотой использования.

            \tparam Value
                Тип элементов исходного массива.
            \tparam Dimension
                Размерность представления.

        \~english
            \brief
                Multi-dimensional view into a single-dimensional array

            \details
                Allows you to work with a linear piece of memory as if it is a multidimensional
                array, that is, an array in an array in an array... etc.

                Usage example:

                \code{.cpp}
                int a[12] = {0};
                // a =
                //  [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

                auto view = burst::make_shaped_array_view(a, burst::make_shape(3, 4));
                // Creates a two-dimensional view consisting of three "rows" of four elements.
                // view =
                //  [0, 0, 0, 0]
                //  [0, 0, 0, 0]
                //  [0, 0, 0, 0]

                view[1][2] = 7;
                // view =
                //  [0, 0, 0, 0]
                //  [0, 0, 7, 0]
                //  [0, 0, 0, 0]
                // a =
                //  [0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0]

                assert(a[4 * 1 + 2] == 7);
                \endcode

                In fact, this is a lightweight version of boost::multi_array_view. The differences
                are only in simplicity of code and ease of use.

            \tparam Value
                Type of elements of the source array.
            \tparam Dimension
                Dimension of view.

        \~  \see make_shaped_array_view
            \see make_shape
            \see boost::multi_array
     */
    template <typename Value, std::size_t Dimension>
    class shaped_array_view;

    /*!
        \~russian
            \brief
                Создать многомерное представление одномерного массива

            \details
                Функция для удобного создания многомерного представления одномерного массива.

                \code{.cpp}
                int a[24] = {0};

                auto view = burst::make_shaped_array_view(a, burst::make_shape(3, 4, 2));
                // Создано трёхмерное представление, состоящее из трёх "таблиц", каждая из которых
                // состоит из четырёх "строк" по два элемента.
                //       [0 0]
                //    [0 0] 0]
                // [0 0] 0] 0]
                // [0 0] 0] 0]
                // [0 0] 0]
                // [0 0]
                //
                // Таким образом, исходный массив разбивается следующим образом:
                // a:                      [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0]
                // view[0..2]:             [0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0]
                // view[....][0..3]:       [0 0 0 0|0 0 0 0|0 0 0 0|0 0 0 0|0 0 0 0|0 0 0 0]
                // view[....][....][0..1]: [0 0|0 0|0 0|0 0|0 0|0 0|0 0|0 0|0 0|0 0|0 0|0 0]
                \endcode

            \param data
                Оборачиваемый массив.
            \param shape
                Форма представления.

            \returns
                Экземпляр класса `shaped_array_view` с разбиением заданной формы.

        \~english
            \brief
                Create a multi-dimensional view into a single-dimensional array

            \details
                Convenience function for creating a multi-dimensional array view.

                \code{.cpp}
                int a[24] = {0};

                auto view = burst::make_shaped_array_view(a, burst::make_shape(3, 4, 2));
                // Creates a three-dimensional view consisting of three "tables" of four "rows"
                // of two elements.
                //       [0 0]
                //    [0 0] 0]
                // [0 0] 0] 0]
                // [0 0] 0] 0]
                // [0 0] 0]
                // [0 0]
                //
                // Thus, the original array is split as follows:
                // a:                      [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0]
                // view[0..2]:             [0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0|0 0 0 0 0 0 0 0]
                // view[....][0..3]:       [0 0 0 0|0 0 0 0|0 0 0 0|0 0 0 0|0 0 0 0|0 0 0 0]
                // view[....][....][0..2]: [0 0|0 0|0 0|0 0|0 0|0 0|0 0|0 0|0 0|0 0|0 0|0 0]
                \endcode

            \param data
                The initial array.
            \param shape
                Shape of the desired view.

            \returns
                An instance of the `shaped_array_view` class with a the specified shape.

        \~  \see shaped_array_view
            \see make_shape
     */
    template <typename Value, typename Integer, std::size_t Dimension>
    auto make_shaped_array_view (Value * data, std::array<Integer, Dimension> shape)
    {
        static_assert(Dimension > 0, u8"Размерность массива должна быть положительной");
        std::partial_sum(shape.rbegin(), shape.rend(), shape.rbegin(), std::multiplies<>{});
        return shaped_array_view<Value, Dimension>(data, detail::cast<std::size_t>(shape));
    }

    /*!
        \~russian
            \brief
                Перегрузка для попытки создания одномерного представления

            \returns data

        \~english
            \brief
                An overload for creating one-dimensional view

            \returns data
     */
    template <typename Value, typename Integer>
    auto make_shaped_array_view (Value * data, const std::array<Integer, 1> &)
    {
        return data;
    }

    /*!
        \~russian
            \brief
                Общий случай многомерного представления

        \~english
            \brief
                A common case of the multidimensional view
     */
    template <typename Value, std::size_t Dimension>
    class shaped_array_view: private detail::shaped_array_view_base<Value, Dimension>
    {
    private:
        using base_type = detail::shaped_array_view_base<Value, Dimension>;

    public:
        shaped_array_view (Value * data, const std::array<std::size_t, Dimension> & shape):
            base_type{data, shape}
        {
        }

        /*!
            \~russian
                \brief
                    Срез по одному элементу верхнего уровня текущего представления

                \details
                    Например, для трёхмерного представления размера `3 x 4 x 2` срез — это
                    таблица `4 x 2`.

                    \code{.cpp}
                    // view =
                    //       [0 0]
                    //    [0 0] 0]
                    // [0 0] 0] 0]
                    // [0 0] 0] 0]
                    // [0 0] 0]
                    // [0 0]
                    //
                    // view[1] =
                    //       [- -]
                    //    [0 0] -]
                    // [- -] 0] -]
                    // [- -] 0] -]
                    // [- -] 0]
                    // [- -]
                    \endcode

                \param index
                    Индекс, по которому нужно произвести срез.

                \returns
                    Представление на одну размерность ниже.

            \~english
                \brief
                    A top-level slice of the current view

                \details
                    For example, for a three-dimensional view of size `3 x 4 x 2`, a top-level
                    slice is table of size `4 x 2`.

                    \code{.cpp}
                    // view =
                    //       [0 0]
                    //    [0 0] 0]
                    // [0 0] 0] 0]
                    // [0 0] 0] 0]
                    // [0 0] 0]
                    // [0 0]
                    //
                    // view[1] =
                    //       [- -]
                    //    [0 0] -]
                    // [- -] 0] -]
                    // [- -] 0] -]
                    // [- -] 0]
                    // [- -]
                    \endcode

                \param index
                    The index by which you want to make a slice.

                \returns
                    One dimension lower view.
         */
        template <typename Integer>
        auto operator [] (Integer index) const
        {
            return
                make_shaped_array_view
                (
                    detail::slice(base_type::data, base_type::shape, index),
                    detail::subarray<1, Dimension>(base_type::shape)
                );
        }
    };

    /*!
        \~russian
            \brief
                Многомерное представление размерности 2

        \~english
            \brief
                A two-dimensional view
     */
    template <typename Value>
    class shaped_array_view<Value, 2>: private detail::shaped_array_view_base<Value, 2>
    {
    private:
        using base_type = detail::shaped_array_view_base<Value, 2>;

    public:
        shaped_array_view (Value * data, const std::array<std::size_t, 2> & shape):
            base_type{data, shape}
        {
        }

        /*!
            \~russian
                \brief
                    Срез по одному элементу верхнего уровня текущего представления

                \returns
                    Указатель на нужное место исходного массива.

            \~english
                \brief
                    A common case of the multidimensional view

                \returns
                    A pointer to some position of the initial array.
         */
        template <typename Integer>
        auto operator [] (Integer index) const
        {
            return detail::slice(base_type::data, base_type::shape, index);
        }
    };

    template <typename... Integers>
    auto make_shape (Integers... ints)
    {
#ifndef NDEBUG
        BURST_EXPAND_VARIADIC(assert(ints > 0));
#endif
        return std::array<std::size_t, sizeof...(Integers)>{static_cast<std::size_t>(ints)...};
    }
} // namespace burst

#endif // BURST_CONTAINER_SHAPED_ARRAY_VIEW_HPP
