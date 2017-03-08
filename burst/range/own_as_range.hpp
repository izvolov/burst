#ifndef BURST_RANGE_OWN_AS_RANGE_HPP
#define BURST_RANGE_OWN_AS_RANGE_HPP

#include <burst/iterator/owning_iterator.hpp>

#include <boost/range/iterator_range.hpp>

#include <utility>

namespace burst
{
    struct own_as_range_t
    {
        //!     Функция для создания диапазона владеющих итераторов
        /*!
                Принимает на вход контейнер, владение над которым нужно установать.
                Возвращает диапазон владеющих итераторов, соответствующих началу и концу заданного
            контейнера.
         */
        template <typename Container>
        auto operator () (Container && container) const
        {
            auto begin = make_owning_iterator(std::forward<Container>(container));
            auto end = make_owning_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }
    };

    constexpr auto own_as_range = own_as_range_t{};
} // namespace burst

#endif // BURST_RANGE_OWN_AS_RANGE_HPP
