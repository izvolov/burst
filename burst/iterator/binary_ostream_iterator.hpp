#ifndef BURST_ITERATOR_BINARY_OSTREAM_ITERATOR_HPP
#define BURST_ITERATOR_BINARY_OSTREAM_ITERATOR_HPP

#include <ostream>
#include <type_traits>

#include <boost/function_output_iterator.hpp>

namespace burst
{
    namespace detail
    {
        template <typename Value>
        struct trivial_write_t
        {
            static_assert(std::is_trivial<Value>::value, "");

            std::ostream & operator () (std::ostream & stream, const Value & value) const
            {
                return
                    stream.write
                    (
                        reinterpret_cast<const char *>(std::addressof(value)),
                        sizeof(value)
                    );
            }
        };
    } // namespace detail

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

    template <typename Value, typename Write = detail::trivial_write_t<Value>>
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
