#ifndef UTIL_HPP
#define UTIL_HPP

#include <type_traits>

#include "type_list.hpp"

namespace turing_machine::util
{
    namespace tl = type_list;

    inline namespace tape
    {
        template <char c>
        struct CharLetter {
            static constexpr auto value = c;
        };
    } // namespace tape

    inline namespace state
    {
        template <char c>
        struct CharState {
            static constexpr auto letter = c;
        };

        template<auto t = []{}>
        struct UniqueState {
        };

    } // namespace state

    template <class Container, class... Args>
    void unpurify(Container &c, tl::List<Args...>)
    {
        static_assert((std::is_constructible_v<typename Container::value_type,
                                               decltype(Args::position),
                                               decltype(Args::letter::value)> && ...));
        (c.push_back({Args::position, Args::letter::value}), ...);
    };
} // namespace turing_machine::util

#endif
