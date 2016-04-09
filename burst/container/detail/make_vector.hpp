#ifndef BURST_CONTAINER_DETAIL_MAKE_VECTOR_HPP
#define BURST_CONTAINER_DETAIL_MAKE_VECTOR_HPP

#include <boost/range/value_type.hpp>

#include <initializer_list>
#include <iterator>
#include <vector>

namespace burst
{
    namespace detail
    {
        template <typename InputRange, typename Allocator>
        auto make_vector_impl (InputRange && values, const Allocator & allocator, std::false_type)
        {
            using value_type = typename boost::range_value<InputRange>::type;
            return
                std::vector<value_type, Allocator>
                (
                    std::begin(values), std::end(values),
                    allocator
                );
        }

        template <typename Value>
        auto make_vector_impl (typename std::vector<Value>::size_type size, const Value & value, std::true_type)
        {
            return std::vector<Value>(size, value);
        }
    } // namespace detail
} // namespace burst

#endif // BURST_CONTAINER_DETAIL_MAKE_VECTOR_HPP
