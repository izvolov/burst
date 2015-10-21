#ifndef BURST_CONTAINER_DYNAMIC_TUPLE_HPP
#define BURST_CONTAINER_DYNAMIC_TUPLE_HPP

#include <algorithm>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <boost/assert.hpp>

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
            m_capacity(std::max(std::size_t{DEFAULT_CAPACITY}, sum({sizeof(Types) + alignof(Types)...}))),
            m_data(std::make_unique<std::int8_t[]>(m_capacity))
        {
            try
            {
                BURST_EXPAND_VARIADIC(push_back_no_realloc(std::forward<Types>(objects)));
            }
            catch (...)
            {
                destroy_all();
                throw;
            }
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
            destroy_all();
        }

        //!     Обеспечить необходимую вместительность контейнера.
        /*!
                Если требуемая вместительность больше текущей, то происходит выделение новой
            памяти, размер которой не меньше (возможно, больше) требуемой. Все элементы контейнера
            перемещаются в новую область памяти, старая память освобождается.
                Если перевыделение памяти произошло, то все ссылки на внутренние объекты становятся
            недействительными.

                Метод не влияет ни на размер, ни на объём контейнера, ни на состояние лежащих
            внутри элементов.

                Сложность: O(size()).
         */
        void reserve (std::size_t new_capacity)
        {
            if (new_capacity > m_capacity)
            {
                new_capacity = std::max(new_capacity, m_capacity * CAPACITY_INCREASING_FACTOR);
                auto new_data = std::make_unique<std::int8_t[]>(new_capacity);
                m_capacity = new_capacity;

                move(0, size(), new_data.get());
                std::swap(m_data, new_data);
            }
        }

        //!     Добавить элемент в конец контейнера.
        /*!
                Кладёт новый элемент в хранилище сразу за текущим последним элементом. Увеличивает
            размер контейнера на единицу, а объём — на размер добавляемого типа.
                Если новый объём превышает вместительность, то происходит перевыделение памяти и
            перемещение элементов в новую область памяти.

                Сложность: O(size()) в худшем случае, O(1) в среднем.
         */
        template <typename T>
        void push_back (T object)
        {
            auto creation_place = set_up_creation_place(object);
            accomodate(std::move(object), creation_place);
        }

        //!     Удалить все элементы из контейнера.
        /*!
                Все ссылки на лежавшие внутри объекты становятся недействительными.
                Вместительность контейнера не изменяется.

                Сложность: O(size()).
         */
        void clear ()
        {
            destroy_all();
            m_offsets.clear();
            m_lifetime_managers.clear();
            m_volume = 0;
        }

        //!     Доступ к элементу по индексу.
        /*!
                Зная тип элемента и его номер в контейнере, можно получить этот элемент.

                Сложность: O(1).
         */
        template <typename T>
        const T & get (size_type index) const
        {
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
        std::size_t volume () const
        {
            return m_volume;
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

        //!     Индикатор пустоты контейнера.
        /*!
                Возвращает истину, если в контейнере нет ни одного элемета, и ложь в противном
            случае.

                Сложность: O(1).
         */
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

        //!     Раздобыть место для создания объекта.
        /*!
                Если в буфере достаточно места для размещения объекта, то сразу возвращает
            указатель на адрес, по которому его можно разместить.
                Если места недостаточно, то увеличивает буфер при помощи метода reserve() до такого
            размера, что в нём уже точно можно разместить объект, и возвращает указатель на адрес,
            по которому его можно разместить.
         */
        template <typename T>
        std::int8_t * set_up_creation_place (const T & object)
        {
            if (auto proposed_creation_place = try_to_align(object))
            {
                return proposed_creation_place;
            }
            else
            {
                reserve(volume() + sizeof(T) + alignof(T));
                return force_align(object);
            }
        }

        //!     Вставка, не производящая перевыделение памяти.
        /*!
                Не производит никаких лишних действий, только размещает объект в буфере.
                Нужна для того случая, когда гарантировано, что в буфере достаточно места для
            размещения нового объекта.
         */
        template <typename T>
        void push_back_no_realloc (T object)
        {
            auto creation_place = force_align(object);
            accomodate(std::move(object), creation_place);
        }

        template <typename T>
        std::int8_t * try_to_align (const T &)
        {
            auto space_left = capacity() - volume();
            void * creation_place = data() + volume();
            return static_cast<std::int8_t *>(std::align(alignof(T), sizeof(T), creation_place, space_left));
        }

        template <typename T>
        std::int8_t * force_align (const T & object)
        {
            auto creation_place = try_to_align(object);
            BOOST_ASSERT(creation_place != nullptr);

            return creation_place;
        }

        //!     Разместить объект.
        /*!
                Кладёт объект в хранилище и создаёт всю необходимую обвязку — отступы, менеджера
            времени жизни, — а также задаёт новый объём с учётом размещённого объекта.
         */
        template <typename T>
        void accomodate (T object, std::int8_t * creation_place)
        {
            new (creation_place) T(std::move(object));

            const auto new_offset = static_cast<std::size_t>(creation_place - data());
            m_offsets.push_back(new_offset);
            m_volume = new_offset + sizeof(T);
            m_lifetime_managers.push_back(std::make_shared<lifetime_manager<T>>());
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

        void destroy_all ()
        {
            destroy(0, size());
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

        std::vector<std::size_t> m_offsets;
        lifetime_manager_container_type m_lifetime_managers;
        std::size_t m_volume = 0;
    };
} // namespace burst

#endif // BURST_CONTAINER_DYNAMIC_TUPLE_HPP
