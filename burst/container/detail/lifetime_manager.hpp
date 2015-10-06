#ifndef BURST_CONTAINER_DETAIL_LIFETIME_MANAGER_HPP
#define BURST_CONTAINER_DETAIL_LIFETIME_MANAGER_HPP

#include <utility>

namespace burst
{
    struct lifetime_manager_base
    {
        virtual ~lifetime_manager_base () = default;
        virtual void move (void * source, void * destination) const = 0;
        virtual void destroy (void * object) const = 0;
    };

    template <typename T>
    struct lifetime_manager: lifetime_manager_base
    {
        void move (void * source, void * destination) const override
        {
            new (destination) T(std::move(*static_cast<T *>(source)));
        }

        void destroy (void * object) const override
        {
            static_cast<T *>(object)->~T();
        }
    };
} // namespace burst

#endif // BURST_CONTAINER_DETAIL_LIFETIME_MANAGER_HPP
