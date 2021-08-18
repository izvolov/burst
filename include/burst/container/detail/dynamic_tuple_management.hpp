#ifndef BURST__CONTAINER__DETAIL__DYNAMIC_TUPLE_MANAGEMENT_HPP
#define BURST__CONTAINER__DETAIL__DYNAMIC_TUPLE_MANAGEMENT_HPP

#include <burst/string/u8s.hpp>

#include <boost/type_index/ctti_type_index.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>
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
            auto type_name = boost::typeindex::ctti_type_index::type_id<T>().pretty_name();
            using literals::operator ""_u8s;
            throw std::runtime_error(u8"Объект типа "_u8s + type_name + u8" некопируем"_u8s);
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
            destroy,
            size,
            type
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
                case operation_t::size:
                {
                    assert(source == nullptr);
                    *static_cast<std::size_t *>(destination) = sizeof(T);
                    break;
                }
                case operation_t::type:
                {
                    assert(source == nullptr);
                    *static_cast<const std::type_info **>(destination) = std::addressof(typeid(T));
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

        inline void destroy (const object_info_t & object, std::int8_t * data)
        {
            object.manage(operation_t::destroy, nullptr, data + object.offset);
        }

        template <typename InputIterator>
        void destroy (InputIterator first, InputIterator last, std::int8_t * data)
        {
            std::for_each(first, last,
                [data] (const auto & object)
                {
                    destroy(object, data);
                });
        }

        template <typename ForwardIterator>
        void
            move
            (
                ForwardIterator first,
                ForwardIterator last,
                std::int8_t * source,
                std::int8_t * destination
            )
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
        void
            copy
            (
                ForwardIterator first,
                ForwardIterator last,
                const std::int8_t * source,
                std::int8_t * destination
            )
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

        //!     Индикатор соответствия менеджера заявленному типу
        template <typename T>
        bool conforms (const object_info_t & object)
        {
            // Адрес функции глобален, поэтому можно просто сравнить указатели.
            return &manage<T> == object.manage;
        }

        inline std::size_t size_of (const object_info_t & object)
        {
            std::size_t size;
            object.manage(operation_t::size, nullptr, std::addressof(size));

            return size;
        }

        inline const std::type_info & type (const object_info_t & object)
        {
            const std::type_info * type;
            object.manage(operation_t::type, nullptr, std::addressof(type));

            return *type;
        }
    } // namespace management
} // namespace burst

#endif // BURST__CONTAINER__DETAIL__DYNAMIC_TUPLE_MANAGEMENT_HPP
