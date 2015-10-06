#ifndef BURST_CONTAINER_DYNAMIC_TUPLE_HPP
#define BURST_CONTAINER_DYNAMIC_TUPLE_HPP

#include <algorithm>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <burst/algorithm/sum.hpp>
#include <burst/container/detail/lifetime_manager.hpp>
#include <burst/variadic.hpp>

namespace burst
{
    class dynamic_tuple
    {
    public:
        using lifetime_manager_container_type = std::vector<std::shared_ptr<lifetime_manager_base>>;
        using size_type = typename lifetime_manager_container_type::size_type;

    public:
        template <typename ... Types>
        explicit dynamic_tuple (Types && ... objects):
            m_capacity(std::max(std::size_t{DEFAULT_CAPACITY}, sum({sizeof(Types)...}))),
            m_data(std::make_unique<std::int8_t[]>(m_capacity))
        {
            BURST_EXPAND_VARIADIC(push_back(std::forward<Types>(objects)));
        }

        dynamic_tuple ():
            m_capacity(DEFAULT_CAPACITY),
            m_data(std::make_unique<std::int8_t[]>(m_capacity))
        {
        }

        dynamic_tuple (dynamic_tuple &&) = default;
        dynamic_tuple & operator = (dynamic_tuple &&) = default;

        dynamic_tuple (const dynamic_tuple &) = delete;
        dynamic_tuple & operator = (const dynamic_tuple &) = delete;

        ~dynamic_tuple ()
        {
            destroy(0, size());
        }

        void reserve (std::size_t new_capacity)
        {
            if (new_capacity > m_capacity)
            {
                new_capacity = std::max(new_capacity, m_capacity * CAPACITY_INCREASING_FACTOR);
                auto new_data = std::make_unique<std::int8_t[]>(new_capacity);

                move(0, size(), new_data.get());
                std::swap(m_data, new_data);
            }
        }

        template <typename T>
        void push_back (T object)
        {
            const auto new_data_size = data_size() + sizeof(T);
            reserve(new_data_size);

            void * creation_place = data() + data_size();
            new (creation_place) T(std::move(object));

            m_offsets.push_back(new_data_size);
            m_lifetime_managers.push_back(std::make_shared<lifetime_manager<T>>());
        }

        //!     Удалить все элементы из контейнера.
        /*!
                Все ссылки на лежавшие внутри объекты становятся недействительными.
                Вместительность контейнера не изменяется.

                Сложность: O(n).
         */
        void clear ()
        {
            destroy(0, size());
            m_offsets.resize(1);
            m_lifetime_managers.clear();
        }

        template <typename T>
        const T & get (size_type index) const
        {
            assert(sizeof(T) == (m_offsets[index + 1] - m_offsets[index]));
            return *static_cast<const T *>(static_cast<const void *>(data() + m_offsets[index]));
        }

        //!     Размер контейнера.
        /*!
                Возвращает количество элементов, лежащих в контейнере.

                Сложность: O(1).
         */
        size_type size () const
        {
            return m_lifetime_managers.size();
        }

        //!     Объём контейнера.
        /*!
                Возвращает суммарный объём памяти, занимаемый элементами, лежащими в контейнере.

                Сложность: O(1).
         */
        std::size_t data_size () const
        {
            return m_offsets.back();
        }

        //!     Вместимость контейнера.
        /*!
                Возвращает размер памяти, выделенной под хранение элементов контейнера.

                Сложность: O(1).
         */
        std::size_t capacity () const
        {
            return m_capacity;
        }

        bool empty () const
        {
            return m_lifetime_managers.empty();
        }

    private:
        std::int8_t * data ()
        {
            return m_data.get();
        }

        const std::int8_t * data () const
        {
            return m_data.get();
        }

        void destroy (std::size_t first, std::size_t last)
        {
            for (auto index = first; index < last; ++index)
            {
                const auto & manager = *m_lifetime_managers[index];
                const auto offset = m_offsets[index];

                manager.destroy(data() + offset);
            }
        }

        void move (std::size_t first, std::size_t last, std::int8_t * new_data)
        {
            for (auto index = first; index < last; ++index)
            {
                const auto & manager = *m_lifetime_managers[index];
                const auto offset = m_offsets[index];

                manager.move(data() + offset, new_data + offset);
                manager.destroy(data() + offset);
            }
        }

        //!     Минимальная вместительность контейнера.
        static const auto DEFAULT_CAPACITY = std::size_t{64};
        //!     Коэффициент роста вместимости контейнера.
        static const auto CAPACITY_INCREASING_FACTOR = std::size_t{2};
        //!     Порог, по достижении которого необходимо уменьшить вместимость.
        static const auto CAPACITY_DECREASING_THRESHOLD = std::size_t{4};

        std::size_t m_capacity = DEFAULT_CAPACITY;
        std::unique_ptr<std::int8_t[]> m_data;

        // В массиве отступов всегда на один элемент больше.
        // m_offsets.back() всегда равен суммарному размеру хранимых объектов, что равно отступу,
        // начиная с которого можно положить следующий объект.
        std::vector<std::size_t> m_offsets = std::vector<std::size_t>({0});
        lifetime_manager_container_type m_lifetime_managers;

    };
} // namespace burst

#endif // BURST_CONTAINER_DYNAMIC_TUPLE_HPP
