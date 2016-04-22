#ifndef BURST_CONTAINER_DETAIL_LIFETIME_MANAGER_HPP
#define BURST_CONTAINER_DETAIL_LIFETIME_MANAGER_HPP

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace burst
{
    namespace lifetime
    {
        template <typename T>
        auto copy (const void * source, void * destination)
            ->
                std::enable_if_t
                <
                    std::is_copy_constructible<T>::value
                >
        {
            new (destination) T(*static_cast<const T *>(source));
        }

        template <typename T>
        auto copy (const void *, void *)
            ->
                std::enable_if_t
                <
                    not std::is_copy_constructible<T>::value
                >
        {
            throw std::runtime_error(u8"Объект некопируем");
        }

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
        using copier_type = void (*) (const void *, void *);
        using mover_type = void (*) (void *, void *);
        using destroyer_type = void (*) (void *);

        copier_type copy;
        mover_type move;
        destroyer_type destroy;
    };

    template <typename T>
    lifetime_manager make_lifetime_manager ()
    {
        return lifetime_manager
        {
            &lifetime::copy<T>,
            &lifetime::move<T>,
            &lifetime::destroy<T>
        };
    }
} // namespace burst

#endif // BURST_CONTAINER_DETAIL_LIFETIME_MANAGER_HPP
