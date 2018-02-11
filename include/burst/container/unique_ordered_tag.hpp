#ifndef BURST_CONTAINER_UNIQUE_ORDERED_TAG_HPP
#define BURST_CONTAINER_UNIQUE_ORDERED_TAG_HPP

namespace burst
{
    namespace container
    {
        //!     Тег упорядоченной неповторяющейся последовательности.
        /*!
                Фиктивный тип, необходимый для того, чтобы указать, что некая последовательность
            является упорядоченной, а элементы в ней ни разу не повторяются.
         */
        struct unique_ordered_tag_t {};

        //!     Константа, обозначающая упорядоченную неповторяющуюся последовательность.
        /*!
                Передаётся в качестве дополнительного параметра, например, в конструктор
            контейнера, если нужно просигнализировать о том, что последовательность, из которой
            конструируется контейнер, уже упорядочена, и в ней нет повторяющихся элементов.
         */
        const unique_ordered_tag_t unique_ordered_tag{};
    } // namespace container
} // namespace burst

#endif // BURST_CONTAINER_UNIQUE_ORDERED_TAG_HPP
