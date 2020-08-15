#ifndef BURST_RANGE_ADAPTOR_BUFFERED_CHUNKED_HPP
#define BURST_RANGE_ADAPTOR_BUFFERED_CHUNKED_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/range/buffered_chunks.hpp>

namespace burst
{
    /*!
        \brief
            Инструмент для получения буферизованного кусочного диапазона

        \details
            Вызов

            \code{.cpp}
            range | buffered_chunked(n)
            \endcode

            эквивалентен вызову

            \code{.cpp}
            buffered_chunks(range, n)
            \endcode

        \see buffered_chunks
     */
    constexpr auto buffered_chunked = make_adaptor_trigger(buffered_chunks);
} // namespace burst

#endif // BURST_RANGE_ADAPTOR_BUFFERED_CHUNKED_HPP
