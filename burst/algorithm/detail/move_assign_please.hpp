#ifndef BURST_ALGORITHM_DETAIL_MOVE_ASSIGN_PLEASE_HPP
#define BURST_ALGORITHM_DETAIL_MOVE_ASSIGN_PLEASE_HPP

#include <burst/iterator/iterator_value.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace burst
{
    namespace detail
    {
        //!     Обеспечить перенос, если возможно
        /*!
                Если итерируемые элементы уважают переносящее присвоение, то преобразовывает
            входной итератор в `std::move_iterator`.
                В противном случае выбрасывает входной итератор без изменений.
         */
        template <typename Iterator>
        constexpr auto move_assign_please (Iterator i)
            ->
                std::enable_if_t
                <
                    std::is_move_assignable<iterator_value_t<Iterator>>::value,
                    std::move_iterator<Iterator>
                >
        {
            return std::make_move_iterator(std::move(i));
        }

        template <typename Iterator>
        constexpr auto move_assign_please (Iterator i)
            ->
                std::enable_if_t
                <
                    not std::is_move_assignable<iterator_value_t<Iterator>>::value,
                    Iterator
                >
        {
            return i;
        }
    } // namespace detail
} // namespace burst

#endif // BURST_ALGORITHM_DETAIL_MOVE_ASSIGN_PLEASE_HPP
