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
    // https://en.wikipedia.org/wiki/Turing_machine_examples#3-state_Busy_Beaver

    using SA = tur::util::CharState<'A'>;
    using SB = tur::util::CharState<'B'>;
    using SC = tur::util::CharState<'C'>;
    using HALT = tur::util::CharState<'H'>;

    using Let0 = tur::Blank;
    using Let1 = tur::util::CharLetter<'1'>;

    using TapeList = tl::EmptyList;

    using StartState = SA;
    using FinalStates = tl::List<HALT>;
    using TransitionTable =
      tl::List<tur::Transition<tur::TransitionFrom<SA, Let0>, tur::TransitionTo<SB, Let1, tur::Direction::R>>,
               tur::Transition<tur::TransitionFrom<SA, Let1>, tur::TransitionTo<SC, Let1, tur::Direction::L>>,
               tur::Transition<tur::TransitionFrom<SB, Let0>, tur::TransitionTo<SA, Let1, tur::Direction::L>>,
               tur::Transition<tur::TransitionFrom<SB, Let1>, tur::TransitionTo<SB, Let1, tur::Direction::R>>,
               tur::Transition<tur::TransitionFrom<SC, Let0>, tur::TransitionTo<SB, Let1, tur::Direction::L>>,
               tur::Transition<tur::TransitionFrom<SC, Let1>, tur::TransitionTo<HALT, Let1, tur::Direction::N>>
               >;
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
