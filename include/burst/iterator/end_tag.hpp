#ifndef BURST__ITERATOR__END_TAG_HPP
#define BURST__ITERATOR__END_TAG_HPP

namespace burst
{
    namespace iterator
    {
        //!     Тег конца итератора.
        /*!
                Фиктивный тип, необходимый для того, чтобы указать, что необходимо создать итератор
            на конец последовательности в том случае, если только по входным данным нельзя сделать
            однозначный вывод о том, будет ли созданный итератор началом или концом
            последовательности.
         */
        struct end_tag_t {};

        //!     Константа, обозначающая конец последовательности.
        /*!
                Передаётся в функцию создания итератора в том случае, если нужно создать итератор
            на конец последовательности.
         */
        const end_tag_t end_tag{};
    }
}

#endif // BURST__ITERATOR__END_TAG_HPP
