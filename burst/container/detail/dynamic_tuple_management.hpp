#ifndef BURST_CONTAINER_DETAIL_DYNAMIC_TUPLE_MANAGEMENT_HPP
#define BURST_CONTAINER_DETAIL_DYNAMIC_TUPLE_MANAGEMENT_HPP

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace burst
{
    namespace management
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

        enum struct operation_t
        {
            copy,
            move,
            destroy
        };

        using manager_t = void (*) (operation_t, const void *, void *);

        template <typename T>
        void manage (operation_t todo, const void * source, void * destination)
        {
            switch (todo)
            {
                case operation_t::copy:
                {
                    copy<T>(source, destination);
                    break;
                }
                case operation_t::move:
                {
                    move<T>(const_cast<void *>(source), destination);
                    break;
                }
                case operation_t::destroy:
                {
                    assert(source == nullptr);
                    destroy<T>(destination);
                    break;
                }
            }
        }

        struct object_info_t
        {
            std::size_t offset;
            manager_t manage;
        };

        template <typename T>
        object_info_t make_object_info (std::size_t offset)
        {
            return object_info_t{offset, &manage<T>};
        }

        template <typename InputIterator>
        void destroy (InputIterator first, InputIterator last, std::int8_t * data)
        {
            while (first != last)
            {
                first->manage(operation_t::destroy, nullptr, data + first->offset);
                ++first;
            }
        }

        template <typename ForwardIterator>
        void move (ForwardIterator first, ForwardIterator last, std::int8_t * source, std::int8_t * destination)
        {
            for (auto current = first; current != last; ++current)
            {
                try
                {
                    current->manage(operation_t::move,
                        source + current->offset, destination + current->offset);
                }
                catch (...)
                {
                    destroy(first, current, destination);
                    throw;
                }
            }
            destroy(first, last, source);
        }

        template <typename ForwardIterator>
        void copy (ForwardIterator first, ForwardIterator last, const std::int8_t * source, std::int8_t * destination)
        {
            for (auto current = first; current != last; ++current)
            {
                try
                {
                    current->manage(operation_t::copy,
                        source + current->offset, destination + current->offset);
                }
                catch (...)
                {
                    destroy(first, current, destination);
                    throw;
                }
            }
        }
    } // namespace management
} // namespace burst

#endif // BURST_CONTAINER_DETAIL_DYNAMIC_TUPLE_MANAGEMENT_HPP
