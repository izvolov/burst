#ifndef BURST_CONTAINER_DETAIL_LIFETIME_MANAGER_HPP
#define BURST_CONTAINER_DETAIL_LIFETIME_MANAGER_HPP

#include <utility>

namespace burst
{
    namespace lifetime
    {
        template <typename T>
        void move (void * source, void * destination)
        {
            new (destination) T(std::move(*static_cast<T *>(source)));
        }

        template <typename T>
        void destroy (void * object)
        {
            static_cast<T *>(object)->~T();
        }
    }

    struct lifetime_manager
    {
        using mover_type = void (*) (void *, void *);
        using destroyer_type = void (*) (void *);

        mover_type move;
        destroyer_type destroy;
    };

    template <typename T>
    lifetime_manager make_lifetime_manager ()
    {
        return lifetime_manager
        {
            &lifetime::move<T>,
            &lifetime::destroy<T>
        };
    }
} // namespace burst

#endif // BURST_CONTAINER_DETAIL_LIFETIME_MANAGER_HPP
