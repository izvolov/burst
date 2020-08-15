#ifndef BURST_RANGE_BUFFERED_CHUNKS_HPP
#define BURST_RANGE_BUFFERED_CHUNKS_HPP

#include <burst/iterator/buffered_chunk_iterator.hpp>
#include <burst/iterator/end_tag.hpp>

#include <boost/range/iterator_range.hpp>

#include <utility>

namespace burst
{
    struct buffered_chunks_t
    {
        /*!
            \brief
                Функция для создания буферизованного кусочного диапазона

            \details
                Создаёт буферизованный кусочный итератор посредством пробрасывания аргументов
                этой функции в функцию `make_buffered_chunk_iterator`, а из этого итератора создаёт
                диапазон.

            \returns
                Диапазон, каждый элемент которого — буферизованный кусок исходного диапазона.
                Буферизованность означает, что кусок исходного диапазона скопирован в отдельную
                область памяти — буфер.

            \see make_buffered_chunk_iterator
            \see buffered_chunk_iterator
         */
        template <typename ... Args>
        auto operator () (Args && ... args) const
        {
            auto begin = make_buffered_chunk_iterator(std::forward<Args>(args)...);
            auto end = make_buffered_chunk_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }
    };

    constexpr auto buffered_chunks = buffered_chunks_t{};
}

#endif // BURST_RANGE_BUFFERED_CHUNKS_HPP
