#ifndef BURST_INTEGER_RIGHT_SHIFT_HPP
#define BURST_INTEGER_RIGHT_SHIFT_HPP

namespace burst
{
    //!     Побитовый сдвиг вправо
    /*!
            Отличается от стандартного оператора только тем, что тип возвращаемого результата
        всегда идентичен типу левого операнда.
     */
    template <typename Integer1, typename Integer2>
    constexpr auto right_shift (Integer1 value, Integer2 shift)
    {
        return static_cast<Integer1>(value >> shift);
    }
} // namespace burst

#endif // BURST_INTEGER_RIGHT_SHIFT_HPP
