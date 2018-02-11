#ifndef BURST_INTEGER_DETAIL_DIVCEIL_HPP
#define BURST_INTEGER_DETAIL_DIVCEIL_HPP

#include <limits>
#include <stdexcept>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        template <typename I>
        constexpr I divceil_positive (I n, I d)
        {
            return static_cast<I>((n - I{1}) / d + I{1});
        }

        template <typename I>
        constexpr I divceil_negative (I n, I d)
        {
            return static_cast<I>(n / d);
        }

        template <typename I, typename J>
        constexpr auto divceil_impl (I n, J d)
            -> std::enable_if_t<std::is_unsigned<I>::value, I>
        {
            if (n > 0)
            {
                using K = std::common_type_t<I, J>;
                return static_cast<I>(divceil_positive(static_cast<K>(n), static_cast<K>(d)));
            }
            else
            {
                return I{0};
            }
        }

        template <typename I, typename J>
        constexpr auto divceil_impl (I n, J d)
            ->
                std::enable_if_t
                <
                    std::is_signed<I>::value &&
                        (std::is_signed<J>::value ||
                            (std::is_unsigned<J>::value
                                && std::is_signed<std::common_type_t<I, J>>::value)),
                    I
                >
        {
            if (n > 0)
            {
                using K = std::common_type_t<I, J>;
                return static_cast<I>(divceil_positive(static_cast<K>(n), static_cast<K>(d)));
            }
            else
            {
                using K = std::common_type_t<I, J>;
                return static_cast<I>(divceil_negative(static_cast<K>(n), static_cast<K>(d)));
            }
        }

        template <typename I, typename J>
        constexpr auto divceil_impl (I n, J d)
            ->
                std::enable_if_t
                <
                    std::is_signed<I>::value &&
                        std::is_unsigned<J>::value
                            && std::is_same<std::common_type_t<I, J>, J>::value,
                    I
                >
        {
            if (n > 0)
            {
                using K = std::common_type_t<I, J>;
                return static_cast<I>(divceil_positive(static_cast<K>(n), static_cast<K>(d)));
            }
            else if (d <= static_cast<J>(std::numeric_limits<I>::max()))
            {
                return divceil_negative(n, static_cast<I>(d));
            }
            else
            {
                return I{0};
            }
        }
    }
}

#endif // BURST_INTEGER_DETAIL_DIVCEIL_HPP
