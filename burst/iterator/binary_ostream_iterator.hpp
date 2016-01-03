#ifndef BURST_ITERATOR_BINARY_OSTREAM_ITERATOR_HPP
#define BURST_ITERATOR_BINARY_OSTREAM_ITERATOR_HPP

#include <burst/iterator/detail/trivial_write.hpp>

#include <boost/function_output_iterator.hpp>

#include <ostream>

namespace burst
{
    template <typename Value, typename Write>
    struct binary_ostream_writer
    {
        void operator () (const Value & value) const
        {
            write(*stream, value);
        }

        std::ostream * stream;
        Write write;
    };

    template <typename Value, typename Write = detail::trivial_write_t>
    auto make_binary_ostream_iterator (std::ostream & stream, Write write = Write{})
    {
        return
            boost::make_function_output_iterator
            (
                binary_ostream_writer<Value, Write>{std::addressof(stream), std::move(write)}
            );
    }
} // namespace burst

#endif // BURST_ITERATOR_BINARY_OSTREAM_ITERATOR_HPP
