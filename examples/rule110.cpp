#include <algorithm>
#include <iostream>
#include <vector>

#include "tm.hpp"
#include "type_list.hpp"
#include "util.hpp"

namespace tur = turing_machine;
namespace tl = type_list;

// |--------|-----------|-----------|-------------|----------|
// | State  | CurSymbol | NextState | WriteSymbol | Movement |
// |--------|-----------|-----------|-------------|----------|
// | init   | S         | l0        | S           | R        |
// | init   | 0         | HALT      | 0           | N        |
// | init   | 1         | HALT      | 1           | N        |
// | init   | E         | HALT      | E           | N        |
// |--------|-----------|-----------|-------------|----------|
// | l0     | 0         | l0m0      | 0           | R        |
// | l0     | 1         | l0m1      | 1           | R        |
// | l0m0   | E         | l0m0r0    | 0           | L        |
// | l0m0   | 0         | l0m0r0    | 0           | L        |
// | l0m0   | 1         | l0m0r1    | 1           | L        |
// | l0m1   | E         | l0m1r0    | 0           | L        |
// | l0m1   | 0         | l0m1r0    | 0           | L        |
// | l0m1   | 1         | l0m1r1    | 1           | L        |
// | l0m0r0 | 0         | l0        | 0           | R        |
// | l0m0r1 | 0         | l0        | 1           | R        |
// | l0m1r0 | 1         | l1        | 1           | R        |
// | l0m1r1 | 1         | l1        | 1           | R        |
// | l1     | 0         | l1m0      | 0           | R        |
// | l1     | 1         | l1m1      | 1           | R        |
// | l1m0   | E         | l1m0r0    | 0           | L        |
// | l1m0   | 0         | l1m0r0    | 0           | L        |
// | l1m0   | 1         | l1m0r1    | 1           | L        |
// | l1m1   | E         | l1m1r0    | 0           | L        |
// | l1m1   | 0         | l1m1r0    | 0           | L        |
// | l1m1   | 1         | l1m1r1    | 1           | L        |
// | l1m0r0 | 0         | l0        | 0           | R        |
// | l1m0r1 | 0         | l0        | 1           | R        |
// | l1m1r0 | 1         | l1        | 1           | R        |
// | l1m1r1 | 1         | l1        | 0           | R        |
// | l0     | E         | HALT      | E           | N        |
// |--------|-----------|-----------|-------------|----------|


using INIT = tur::util::CharState<'i'>;

using L0 = tur::util::UniqueState<>;
using L1 = tur::util::UniqueState<>;
using L0M0 = tur::util::UniqueState<>;
using L0M1 = tur::util::UniqueState<>;
using L1M0 = tur::util::UniqueState<>;
using L1M1 = tur::util::UniqueState<>;
using L0M0R0 = tur::util::UniqueState<>;
using L0M0R1 = tur::util::UniqueState<>;
using L0M1R0 = tur::util::UniqueState<>;
using L0M1R1 = tur::util::UniqueState<>;
using L1M0R0 = tur::util::UniqueState<>;
using L1M0R1 = tur::util::UniqueState<>;
using L1M1R0 = tur::util::UniqueState<>;
using L1M1R1 = tur::util::UniqueState<>;

using HALT = tur::util::CharState<'h'>;

using START = tur::util::CharLetter<'S'>;
using LET0 = tur::util::CharLetter<' '>;
using LET1 = tur::util::CharLetter<'#'>;
using END = tur::util::CharLetter<'E'>;

using TransitionTable =
  tl::List<tur::Transition<tur::TransitionFrom<INIT, START>, tur::TransitionTo<L0, START, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<INIT, LET0>, tur::TransitionTo<HALT, LET0, tur::Direction::N>>,
           tur::Transition<tur::TransitionFrom<INIT, LET1>, tur::TransitionTo<HALT, LET1, tur::Direction::N>>,
           tur::Transition<tur::TransitionFrom<INIT, END>, tur::TransitionTo<HALT, END, tur::Direction::N>>,

           tur::Transition<tur::TransitionFrom<L0, LET0>, tur::TransitionTo<L0M0, LET0, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L0, LET1>, tur::TransitionTo<L0M1, LET1, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L0M0, END>, tur::TransitionTo<L0M0R0, END, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L0M0, LET0>, tur::TransitionTo<L0M0R0, LET0, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L0M0, LET1>, tur::TransitionTo<L0M0R1, LET1, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L0M1, END>, tur::TransitionTo<L0M1R0, END, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L0M1, LET0>, tur::TransitionTo<L0M1R0, LET0, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L0M1, LET1>, tur::TransitionTo<L0M1R1, LET1, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L0M0R0, LET0>, tur::TransitionTo<L0, LET0, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L0M0R1, LET0>, tur::TransitionTo<L0, LET1, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L0M1R0, LET1>, tur::TransitionTo<L1, LET1, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L0M1R1, LET1>, tur::TransitionTo<L1, LET1, tur::Direction::R>>,

           tur::Transition<tur::TransitionFrom<L1, LET0>, tur::TransitionTo<L1M0, LET0, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L1, LET1>, tur::TransitionTo<L1M1, LET1, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L1M0, END>, tur::TransitionTo<L1M0R0, END, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L1M0, LET0>, tur::TransitionTo<L1M0R0, LET0, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L1M0, LET1>, tur::TransitionTo<L1M0R1, LET1, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L1M1, END>, tur::TransitionTo<L1M1R0, END, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L1M1, LET0>, tur::TransitionTo<L1M1R0, LET0, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L1M1, LET1>, tur::TransitionTo<L1M1R1, LET1, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L1M0R0, LET0>, tur::TransitionTo<L0, LET0, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L1M0R1, LET0>, tur::TransitionTo<L0, LET1, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L1M1R0, LET1>, tur::TransitionTo<L1, LET1, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L1M1R1, LET1>, tur::TransitionTo<L1, LET0, tur::Direction::R>>,

           tur::Transition<tur::TransitionFrom<L0, END>, tur::TransitionTo<HALT, END, tur::Direction::N>>,
           tur::Transition<tur::TransitionFrom<L1, END>, tur::TransitionTo<HALT, END, tur::Direction::N>>>;

using StartState = INIT;
using FinalStates = tl::List<HALT>;

template <class Tape>
using Rule110Machine = tur::Machine<StartState, FinalStates, TransitionTable, Tape>;

template <class InitTape, std::size_t N>
struct Rule110 {
    using PrevTape = typename Rule110<InitTape, N - 1>::Tape;
    using Tape = typename tur::Run<Rule110Machine<PrevTape>>::tape;
};

template <class InitTape>
struct Rule110<InitTape, 0> {
    using Tape = InitTape;
};

void rule110_helper(std::vector<std::pair<int, char>> &v)
{
    auto it = std::remove_if(v.begin(), v.end(), [](const auto &a) {
        return a.second == 'S' || a.second == 'E';
    });
    v.erase(it, v.end());
    std::sort(v.begin(), v.end(), [](const auto &a, const auto &b) {
        return a.first < b.first;
    });
    for (const auto &el : v) {
        std::cout << el.second;
    }
    std::cout << std::endl;
}

template <class Tape, std::size_t... levels>
void rule110(std::integer_sequence<std::size_t, levels...>)
{
    std::vector<std::pair<int, char>> v;

    ((v.clear(), tur::util::unpurify(v, typename Rule110<Tape, levels>::Tape{}), rule110_helper(v)), ...);
}

int main()
{
    using TapeList = tl::List<tur::TapeCell<0, START>,
                              tur::TapeCell<1, LET0>,
                              tur::TapeCell<2, LET0>,
                              tur::TapeCell<3, LET0>,
                              tur::TapeCell<4, LET0>,
                              tur::TapeCell<5, LET0>,
                              tur::TapeCell<6, LET0>,
                              tur::TapeCell<7, LET0>,
                              tur::TapeCell<8, LET0>,
                              tur::TapeCell<9, LET0>,
                              tur::TapeCell<10, LET1>,
                              tur::TapeCell<11, END>>;

    rule110<TapeList>(std::make_index_sequence<10>{});

    return 0;
}
