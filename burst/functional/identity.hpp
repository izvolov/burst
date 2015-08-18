#ifndef BURST_FUNCTIONAL_IDENTITY_HPP
#define BURST_FUNCTIONAL_IDENTITY_HPP

namespace burst
{
    //!     Тождественное отображение, параметризованное типом.
    /*!
            Возвращает то же самое значение, которое принимает на вход.
            Тип значения задан в типе отображения.
     */
    template <typename Type = void>
    struct identity
    {
        const Type & operator () (const Type & value) const
        {
            return value;
        }
    };

    //!     Тождественное отображение.
    /*!
            Возвращает то же самое значение, что и принимает на вход.
            В отличие от аналогичного функционального объекта, параметризованного типом значения,
        не зависит от типа принимаемого значения.
     */
    template <>
    struct identity <void>
    {
        template <typename Type>
        constexpr decltype(auto) operator () (Type && value) const
        {
            return std::forward<Type>(value);
        }
    };
}

#endif // BURST_FUNCTIONAL_IDENTITY_HPP
