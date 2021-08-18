#ifndef BURST__ITERATOR__BINARY_OSTREAM_ITERATOR_HPP
#define BURST__ITERATOR__BINARY_OSTREAM_ITERATOR_HPP

#include <burst/functional/trivial_write.hpp>

#include <boost/function_output_iterator.hpp>

#include <memory>
#include <ostream>
#include <utility>

namespace burst
{
    template <typename Write>
    struct binary_ostream_writer
    {
        template <typename T>
        void operator () (T && value) const
        {
            write(*stream, std::forward<T>(value));
        }

        std::ostream * stream;
        Write write;
    };

    template <typename Write = trivial_write_fn>
    auto make_binary_ostream_iterator (std::ostream & stream, Write write = Write{})
    {
        return
            boost::make_function_output_iterator
            (
                binary_ostream_writer<Write>{std::addressof(stream), std::move(write)}
            );
    }
} // namespace burst

#endif // BURST__ITERATOR__BINARY_OSTREAM_ITERATOR_HPP
