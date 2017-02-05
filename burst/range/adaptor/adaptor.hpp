#ifndef BURST_RANGE_ADAPTOR_ADAPTOR_HPP
#define BURST_RANGE_ADAPTOR_ADAPTOR_HPP

#include <type_traits>
#include <utility>

namespace burst
{
    template <typename Adaptor, typename Compare>
    struct adaptor_compare_forwarder_t
    {
        Adaptor adaptor;
        Compare compare;
    };

    template <typename Adaptor, typename Compare>
    constexpr auto make_adaptor_forwarder (Adaptor && adaptor, Compare && compare)
        -> adaptor_compare_forwarder_t<std::decay_t<Adaptor>, std::decay_t<Compare>>
    {
        return {std::forward<Adaptor>(adaptor), std::forward<Compare>(compare)};
    }

    template <typename Adaptor>
    struct adaptor_trigger_t
    {
        template <typename Compare>
        constexpr auto operator () (Compare && compare) const
        {
            return make_adaptor_forwarder(adaptor, std::forward<Compare>(compare));
        }

        Adaptor adaptor;
    };

    template <typename Adaptor>
    constexpr auto make_adaptor_trigger (Adaptor && adaptor)
        -> adaptor_trigger_t<std::decay_t<Adaptor>>
    {
        return {std::forward<Adaptor>(adaptor)};
    }

    template <typename Range, typename Adaptor, typename Compare>
    auto operator | (Range && range, adaptor_compare_forwarder_t<Adaptor, Compare> forwarder)
    {
        return forwarder.adaptor(std::forward<Range>(range), std::move(forwarder.compare));
    }

    template <typename Range, typename Adaptor>
    auto operator | (Range && range, adaptor_trigger_t<Adaptor> trigger)
    {
        return trigger.adaptor(std::forward<Range>(range));
    }
} // namespace burst

#endif // BURST_RANGE_ADAPTOR_ADAPTOR_HPP
