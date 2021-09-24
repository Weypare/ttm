#include <algorithm>
#include <iostream>
#include <vector>

#include "tm.hpp"
#include "type_list.hpp"
#include "util.hpp"

namespace tur = turing_machine;
namespace tl = type_list;

int main()
{
    // Source:
    // https://en.wikipedia.org/wiki/Turing_machine_examples#A_copy_subroutine

    using S1 = tur::util::CharState<'1'>;
    using S2 = tur::util::CharState<'2'>;
    using S3 = tur::util::CharState<'3'>;
    using S4 = tur::util::CharState<'4'>;
    using S5 = tur::util::CharState<'5'>;
    using HALT = tur::util::CharState<'h'>;

    using Blank = tur::Blank;
    using Let1 = tur::util::CharLetter<'1'>;

    using TapeList =
      tl::List<tur::TapeCell<0, Let1>, tur::TapeCell<1, Let1>, tur::TapeCell<2, Let1>, tur::TapeCell<3, Let1>>;

    using StartState = S1;
    using FinalStates = tl::List<HALT>;
    using TransitionTable =
      tl::List<tur::Transition<tur::TransitionFrom<S1, Blank>, tur::TransitionTo<HALT, Blank, tur::Direction::N>>,
               tur::Transition<tur::TransitionFrom<S1, Let1>, tur::TransitionTo<S2, Blank, tur::Direction::R>>,
               tur::Transition<tur::TransitionFrom<S2, Blank>, tur::TransitionTo<S3, Blank, tur::Direction::R>>,
               tur::Transition<tur::TransitionFrom<S2, Let1>, tur::TransitionTo<S2, Let1, tur::Direction::R>>,
               tur::Transition<tur::TransitionFrom<S3, Blank>, tur::TransitionTo<S4, Let1, tur::Direction::L>>,
               tur::Transition<tur::TransitionFrom<S3, Let1>, tur::TransitionTo<S3, Let1, tur::Direction::R>>,
               tur::Transition<tur::TransitionFrom<S4, Blank>, tur::TransitionTo<S5, Blank, tur::Direction::L>>,
               tur::Transition<tur::TransitionFrom<S4, Let1>, tur::TransitionTo<S4, Let1, tur::Direction::L>>,
               tur::Transition<tur::TransitionFrom<S5, Blank>, tur::TransitionTo<S1, Let1, tur::Direction::R>>,
               tur::Transition<tur::TransitionFrom<S5, Let1>, tur::TransitionTo<S5, Let1, tur::Direction::L>>>;
    using CopyMachine = tur::Machine<StartState, FinalStates, TransitionTable, TapeList>;
    using Final = tur::Run<CopyMachine>;
    static_assert(tl::Contains<Final::state, FinalStates>, "Turing Machine is not in accepting state");

    std::vector<std::pair<int, char>> v;
    tur::util::unpurify(v, Final::tape{});

    std::sort(v.begin(), v.end(), [](const auto &a, const auto &b) {
        return a.first < b.first;
    });
    for (const auto &el : v) {
        std::cout << el.first << ": " << el.second << std::endl;
    }

    return 0;
}
