#ifndef BURST_CONTAINER_DETAIL_MAKE_SEQUENCE_CONTAINER_HPP
#define BURST_CONTAINER_DETAIL_MAKE_SEQUENCE_CONTAINER_HPP

#include <boost/range/value_type.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace burst
{
    namespace detail
    {
        // Диспетчеризация похожих по сигнатуре случаев.

        template
        <
            template <typename ...> class SequenceContainer,
            typename InputRange,
            typename Allocator
        >
        auto
            make_sequence_container_impl
            (
                InputRange && values,
                const Allocator & allocator,
                std::false_type
            )
        {
            using value_type = typename boost::range_value<InputRange>::type;
            return
                SequenceContainer<value_type, Allocator>
                (
                    std::begin(std::forward<InputRange>(values)),
                    std::end(std::forward<InputRange>(values)),
                    allocator
                );
        }

        template <template <typename ...> class SequenceContainer, typename Value>
        auto
            make_sequence_container_impl
            (
                typename SequenceContainer<Value>::size_type size,
                const Value & value,
                std::true_type
            )
        {
            return SequenceContainer<Value>(size, value);
        }
    } // namespace detail
} // namespace burst

#endif // BURST_CONTAINER_DETAIL_MAKE_SEQUENCE_CONTAINER_HPP
