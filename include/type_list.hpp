#ifndef TYPELIST_HPP
#define TYPELIST_HPP

#include <type_traits>

namespace type_list
{
    template <class... Xs>
    struct List {
    };
    using EmptyList = List<>;

    namespace detail
    {
        template <class X, class... Args>
        auto append(List<Args...>) -> List<X, Args...>;
        template <class X, class... Args>
        auto head(List<X, Args...>) -> X;
        template <class X, class... Args>
        auto tail(List<X, Args...>) -> List<Args...>;
    } // namespace detail

    template <class T, class List>
    using AppendList = decltype(detail::append<T>(List{}));

    template <class List>
    using Head = decltype(detail::head(List{}));

    template <class List>
    using Tail = decltype(detail::tail(List{}));

    namespace detail
    {
        template <class X, class L>
        struct Contains;
        template <class X>
        struct Contains<X, EmptyList> {
            static constexpr auto value = false;
        };
        template <class X, class L>
        struct Contains {
            static constexpr auto value = std::is_same_v<X, Head<L>> || Contains<X, Tail<L>>::value;
        };
    } // namespace detail

    template <class X, class List>
    constexpr auto Contains = detail::Contains<X, List>::value;

    namespace detail
    {
        template <class Before, class After, class List>
        struct Replace;
        template <class Before, class After>
        struct Replace<Before, After, EmptyList> {
            using value = EmptyList;
        };
        template <class Before, class After, class List>
        struct Replace {
            using list_value = std::conditional_t<std::is_same_v<Head<List>, Before>, After, Head<List>>;
            using value = AppendList<list_value, typename Replace<Before, After, Tail<List>>::value>;
        };
    } // namespace detail

    template <class Before, class After, class List>
    using Replace = typename detail::Replace<Before, After, List>::value;
} // namespace type_list

#endif
