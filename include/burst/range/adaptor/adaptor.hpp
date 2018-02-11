#ifndef BURST_RANGE_ADAPTOR_ADAPTOR_HPP
#define BURST_RANGE_ADAPTOR_ADAPTOR_HPP

#include <burst/tuple/apply.hpp>
#include <burst/tuple/forward_tuple.hpp>

#include <tuple>
#include <type_traits>
#include <utility>

namespace burst
{
    template <typename Adaptor, typename Tuple>
    struct adaptor_compare_forwarder_t
    {
        Adaptor adaptor;
        Tuple arguments;
    };

    template <typename Adaptor, typename ... Arguments>
    constexpr auto make_adaptor_forwarder (Adaptor && adaptor, Arguments && ... arguments)
        ->
            adaptor_compare_forwarder_t
            <
                std::decay_t<Adaptor>,
                decltype(std::make_tuple(std::forward<Arguments>(arguments)...))
            >
    {
        return
            {
                std::forward<Adaptor>(adaptor),
                std::make_tuple(std::forward<Arguments>(arguments)...)
            };
    }

    template <typename Adaptor>
    struct adaptor_trigger_t
    {
        template <typename ... Arguments>
        constexpr auto operator () (Arguments && ... arguments) const
        {
            return make_adaptor_forwarder(adaptor, std::forward<Arguments>(arguments)...);
        }

        Adaptor adaptor;
    };

    template <typename Adaptor>
    constexpr auto make_adaptor_trigger (Adaptor && adaptor)
        -> adaptor_trigger_t<std::decay_t<Adaptor>>
    {
        return {std::forward<Adaptor>(adaptor)};
    }

    template <typename Range, typename Adaptor, typename Tuple>
    auto operator | (Range && range, adaptor_compare_forwarder_t<Adaptor, Tuple> forwarder)
    {
        return
            apply
            (
                forwarder.adaptor,
                std::tuple_cat
                (
                    std::forward_as_tuple(std::forward<Range>(range)),
                    forward_tuple(std::move(forwarder.arguments))
                )
            );
    }

    template <typename Range, typename Adaptor>
    auto operator | (Range && range, adaptor_trigger_t<Adaptor> trigger)
    {
        return trigger.adaptor(std::forward<Range>(range));
    }
} // namespace burst

#endif // BURST_RANGE_ADAPTOR_ADAPTOR_HPP
