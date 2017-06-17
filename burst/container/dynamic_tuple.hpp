#ifndef BURST_CONTAINER_DYNAMIC_TUPLE_HPP
#define BURST_CONTAINER_DYNAMIC_TUPLE_HPP

#include <burst/algorithm/sum.hpp>
#include <burst/container/detail/dynamic_tuple_management.hpp>
#include <burst/variadic.hpp>

#include <boost/assert.hpp>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace burst
{
    //!     Динамический кортеж.
    /*!
            Неоднородный контейнер с плотной упаковкой. Позволяет складывать в один контейнер
        разнородные объекты и доставать их оттуда, зная их типы и порядковые номера.

            Ключевые особенности:
            1. Складываемые в контейнер типы не обязаны быть членами одной иерархии классов — это
        могут быть совершенно разные типы. Например, можно поместить одновременно bool, double,
        std::string, std::vector<int> и т.п.
            2. Объекты плотно расположены в памяти. То есть они лежат в едином буфере подряд, друг
        за другом. Это снимает лишний уровень косвенности при обращении к ним по сравнению с тем,
        если бы это был, например, массив указателей на базовый класс, как это обычно делается для
        хранения в массиве классов из одной иерархии.
     */
    class dynamic_tuple
    {
    private:
        using object_info_container_type = std::vector<management::object_info_t>;

    public:
        using size_type = typename object_info_container_type::size_type;

    public:
        template <typename ... Types>
        explicit dynamic_tuple (Types ... objects):
            m_capacity(sum({sizeof(Types) + alignof(Types)...})),
            m_data(std::make_unique<std::int8_t[]>(m_capacity))
        {
            try
            {
                BURST_EXPAND_VARIADIC(push_back_no_realloc(std::move(objects)));
            }
            catch (...)
            {
                destroy_all();
                throw;
            }
        }

        dynamic_tuple () = default;

        dynamic_tuple (dynamic_tuple &&) = default;

        dynamic_tuple & operator = (dynamic_tuple && that)
        {
            this->swap(that);
            return *this;
        }

        dynamic_tuple (const dynamic_tuple & that):
            m_capacity(that.m_capacity),
            m_data(std::make_unique<std::int8_t[]>(m_capacity)),
            m_objects(that.m_objects),
            m_volume(that.m_volume)
        {
            management::copy(m_objects.begin(), m_objects.end(), that.data(), this->data());
        }

        dynamic_tuple & operator = (const dynamic_tuple & that)
        {
            return *this = dynamic_tuple(that);
        }

        ~dynamic_tuple ()
        {
            destroy_all();
        }

        void swap (dynamic_tuple & that)
        {
            using std::swap;

            swap(this->m_capacity, that.m_capacity);
            swap(this->m_data, that.m_data);
            swap(this->m_objects, that.m_objects);
            swap(this->m_volume, that.m_volume);
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
                reallocate(std::max(new_capacity, m_capacity * CAPACITY_INCREASING_FACTOR));
            }
        }

        //!     Сократить используемую память до необходимого минимума
        /*!
                Если кортеж использует не всю выделенную память, то есть вместительность больше его
            объёма, то перекладывает внутренние объекты в новую область памяти, размер которой
            строго равен объёму. Соответственно, после выполнения этой функции вместительность
            кортежа становится равной его объёму.
                Если перевыделение памяти произошло, то все ссылки на внутренние объекты становятся
            недействительными.

                Метод не влияет ни на размер, ни на объём контейнера, ни на состояние лежащих
            внутри элементов.

                Сложность: O(size()).
         */
        void shrink_to_fit ()
        {
            if (m_capacity > m_volume)
            {
                reallocate(m_volume);
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
        void push_back (T && object)
        {
            auto creation_place = set_up_creation_place(object);
            accomodate(std::forward<T>(object), creation_place);
        }

        //!     Удалить последний элемент контейнера.
        /*!
                Разрушает последний элемент в кортеже, уменьшает размер на единицу, а объём делает
            равным объёму кортежа, составленного из всех элементов, кроме последнего.
                Вместительность не изменяется.

                Сложность: O(1).
         */
        void pop_back ()
        {
            management::destroy(m_objects.back(), data());
            m_objects.pop_back();

            if (not m_objects.empty())
            {
                auto offset = m_objects.back().offset;
                auto size = management::size_of(m_objects.back());

                m_volume = offset + size;
            }
            else
            {
                m_volume = 0;
            }
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
            m_objects.clear();
            m_volume = 0;
        }

        //!     Доступ к неизменяемому элементу по индексу.
        /*!
                Зная тип элемента и его номер в контейнере, можно получить этот элемент.

                Сложность: O(1).
         */
        template <typename T>
        const T & get (size_type index) const
        {
            assert(&management::manage<T> == m_objects[index].manage);
            return *static_cast<const T *>(static_cast<const void *>(data() + offset(index)));
        }

        //!     Доступ к изменяемому элементу по индексу.
        /*!
                Сложность: O(1).
         */
        template <typename T>
        T & get (size_type index)
        {
            assert(&management::manage<T> == m_objects[index].manage);
            return *static_cast<T *>(static_cast<void *>(data() + offset(index)));
        }

        //!     Отступ элемента, находящегося по заданному индексу.
        /*!
                Возвращает отступ элемента от начала хранилища.
                В дальнейшем может использоваться для передачи в метод `get_by_offset`.

                Сложность: O(1).
         */
        size_type offset (size_type index) const
        {
            return m_objects[index].offset;
        }

        //!     Доступ к неизменяемому элементу по отступу.
        /*!
                Более эффективен, чем доступ по индексу, поскольку при доступе по индексу всё равно
            придётся вычислять отступ. А здесь он дан изначально.

                Сложность: O(1).
         */
        template <typename T>
        const T & get_by_offset (size_type offset) const
        {
            return *static_cast<const T *>(static_cast<const void *>(data() + offset));
        }

        //!     Доступ к изменяемому элементу по отступу.
        /*!
                Сложность: O(1).
         */
        template <typename T>
        T & get_by_offset (size_type offset)
        {
            return *static_cast<T *>(static_cast<void *>(data() + offset));
        }

        //!     Размер контейнера.
        /*!
                Возвращает количество элементов, лежащих в контейнере.

                Сложность: O(1).
         */
        size_type size () const
        {
            return m_objects.size();
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
            return m_objects.empty();
        }

        //!     Информация о типе объекта по индексу.
        /*!
                Сложность: O(1).
         */
        const std::type_info & type (size_type index) const
        {
            return management::type(m_objects[index]);
        }

    private:
        //!     Переразместить данные в новой области памяти.
        /*!
                Выделяет новую область памяти заданного размера и переносит все объекты из старой
            области в новую. Старая область памяти освобождается.

                Сложность: O(size()).
         */
        void reallocate (std::size_t new_capacity)
        {
            assert(new_capacity >= m_volume);
            auto new_data = std::make_unique<std::int8_t[]>(new_capacity);
            m_capacity = new_capacity;

            management::move(m_objects.begin(), m_objects.end(), data(), new_data.get());
            std::swap(m_data, new_data);
        }

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
        void push_back_no_realloc (T && object)
        {
            auto creation_place = force_align(object);
            accomodate(std::forward<T>(object), creation_place);
        }

        //!     Попытаться разместить объект в имеющемся буфере.
        /*!
                Если объект можно разместить, то возвращается адрес, по которому это можно сделать.
                Если же нельзя, то возвращается нулевой указатель.
         */
        template <typename T>
        std::int8_t * try_to_align (const T &)
        {
            auto space_left = capacity() - volume();
            void * creation_place = data() + volume();
            auto aligned_place = std::align(alignof(T), sizeof(T), creation_place, space_left);
            return static_cast<std::int8_t *>(aligned_place);
        }

        //!     Разместить объект в имеющемся буфере.
        /*!
                Предусловие этой функции — в буфере достаточно места для размещения объекта, то
            есть подразумевается, что пользователь уверен, что места достаточно и возвращённый
            указатель будет ненулевым.
         */
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
        void accomodate (T && object, std::int8_t * creation_place)
        {
            using raw_type = std::decay_t<T>;
            new (creation_place) raw_type(std::forward<T>(object));

            const auto new_offset = static_cast<std::size_t>(creation_place - data());
            m_objects.push_back(management::make_object_info<raw_type>(new_offset));
            m_volume = new_offset + sizeof(raw_type);
        }

        void destroy_all ()
        {
            management::destroy(m_objects.begin(), m_objects.end(), data());
        }

        //!     Коэффициент роста вместимости контейнера.
        static const auto CAPACITY_INCREASING_FACTOR = std::size_t{2};
        //!     Порог, по достижении которого необходимо уменьшить вместимость.
        static const auto CAPACITY_DECREASING_THRESHOLD = std::size_t{4};

        std::size_t m_capacity = 0;
        std::unique_ptr<std::int8_t[]> m_data;

        object_info_container_type m_objects;
        std::size_t m_volume = 0;
    };

    inline void swap (dynamic_tuple & left, dynamic_tuple & right)
    {
        left.swap(right);
    }
} // namespace burst

#endif // BURST_CONTAINER_DYNAMIC_TUPLE_HPP
