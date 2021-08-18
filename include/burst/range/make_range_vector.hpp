#ifndef BURST__RANGE__MAKE_RANGE_VECTOR_HPP
#define BURST__RANGE__MAKE_RANGE_VECTOR_HPP

#include <burst/container/make_vector.hpp>

#include <boost/range/iterator_range.hpp>

namespace burst
{
    struct make_range_vector_t
    {
        //!     Превратить набор коллекций в std::vector диапазонов.
        /*!
                Принимает произвольное множество коллекций одного типа, создаёт для них диапазоны и
            возвращает вектор этих диапазонов в соответствующем порядке.
         */
        template <typename ... Collections>
        auto operator () (Collections & ... collections) const
        {
            return make_vector({boost::make_iterator_range(collections)...});
        }
    };

    constexpr auto make_range_vector = make_range_vector_t{};
} // namespace burst

#endif // BURST__RANGE__MAKE_RANGE_VECTOR_HPP
