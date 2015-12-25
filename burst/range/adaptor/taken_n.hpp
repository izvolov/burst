#ifndef BURST_RANGE_ADAPTOR_TAKEN_N_HPP
#define BURST_RANGE_ADAPTOR_TAKEN_N_HPP

#include <type_traits>

#include <boost/range/difference_type.hpp>

#include <burst/range/take_n.hpp>

namespace burst
{
    namespace detail
    {
        template <typename Integer>
        struct item_count_forwarder
        {
            Integer value;
        };

        template <typename Range>
        auto
            operator |
            (
                Range & range,
                detail::item_count_forwarder<typename boost::range_difference<Range>::type> n
            )
        {
            return take_n(range, n.value);
        }
    }

    //!     Инструмент для откусывания диапазона через конвейер.
    /*!
            Вызов

                `range | taken_n(10)`

            эквивалентен вызову

                `take_n(range, 10)`.
     */
    template <typename Integer>
    auto taken_n (Integer n)
    {
        static_assert(std::is_integral<Integer>::value, "");
        return detail::item_count_forwarder<Integer>{n};
    };
} // namespace burst

#endif // BURST_RANGE_ADAPTOR_TAKEN_N_HPP
