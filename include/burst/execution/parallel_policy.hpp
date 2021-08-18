#ifndef BURST__EXECUTION__PARALLEL_POLICY_HPP
#define BURST__EXECUTION__PARALLEL_POLICY_HPP

#include <cassert>
#include <cstddef>

namespace burst
{
    /*!
        \brief
            Тег, используемый для перегрузки функций, чтобы указать, что нужно вызвать параллельный
            вариант алгоритма
     */
    struct parallel_policy
    {
        //! Желаемое количество потоков для параллелизации.
        std::size_t thread_count;
    };

    template <typename Integral>
    constexpr auto par (Integral n)
    {
        assert(n >= 0);
        return parallel_policy{static_cast<std::size_t>(n)};
    }
} // namespace burst

#endif // BURST__EXECUTION__PARALLEL_POLICY_HPP
