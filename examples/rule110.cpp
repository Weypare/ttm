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

// clang-format off
using TransitionTable =
  tl::List<
           tur::Transition<tur::TransitionFrom<INIT, START>, tur::TransitionTo<L0,   START, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<INIT, LET0>,  tur::TransitionTo<HALT, LET0,  tur::Direction::N>>,
           tur::Transition<tur::TransitionFrom<INIT, LET1>,  tur::TransitionTo<HALT, LET1,  tur::Direction::N>>,
           tur::Transition<tur::TransitionFrom<INIT, END>,   tur::TransitionTo<HALT, END,   tur::Direction::N>>,

           tur::Transition<tur::TransitionFrom<L0,     LET0>, tur::TransitionTo<L0M0,   LET0, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L0,     LET1>, tur::TransitionTo<L0M1,   LET1, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L0M0,   END>,  tur::TransitionTo<L0M0R0, END,  tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L0M0,   LET0>, tur::TransitionTo<L0M0R0, LET0, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L0M0,   LET1>, tur::TransitionTo<L0M0R1, LET1, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L0M1,   END>,  tur::TransitionTo<L0M1R0, END,  tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L0M1,   LET0>, tur::TransitionTo<L0M1R0, LET0, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L0M1,   LET1>, tur::TransitionTo<L0M1R1, LET1, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L0M0R0, LET0>, tur::TransitionTo<L0,     LET0, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L0M0R1, LET0>, tur::TransitionTo<L0,     LET1, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L0M1R0, LET1>, tur::TransitionTo<L1,     LET1, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L0M1R1, LET1>, tur::TransitionTo<L1,     LET1, tur::Direction::R>>,

           tur::Transition<tur::TransitionFrom<L1,     LET0>, tur::TransitionTo<L1M0,   LET0, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L1,     LET1>, tur::TransitionTo<L1M1,   LET1, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L1M0,   END>,  tur::TransitionTo<L1M0R0, END,  tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L1M0,   LET0>, tur::TransitionTo<L1M0R0, LET0, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L1M0,   LET1>, tur::TransitionTo<L1M0R1, LET1, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L1M1,   END>,  tur::TransitionTo<L1M1R0, END,  tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L1M1,   LET0>, tur::TransitionTo<L1M1R0, LET0, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L1M1,   LET1>, tur::TransitionTo<L1M1R1, LET1, tur::Direction::L>>,
           tur::Transition<tur::TransitionFrom<L1M0R0, LET0>, tur::TransitionTo<L0,     LET0, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L1M0R1, LET0>, tur::TransitionTo<L0,     LET1, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L1M1R0, LET1>, tur::TransitionTo<L1,     LET1, tur::Direction::R>>,
           tur::Transition<tur::TransitionFrom<L1M1R1, LET1>, tur::TransitionTo<L1,     LET0, tur::Direction::R>>,

           tur::Transition<tur::TransitionFrom<L0, END>, tur::TransitionTo<HALT, END, tur::Direction::N>>,
           tur::Transition<tur::TransitionFrom<L1, END>, tur::TransitionTo<HALT, END, tur::Direction::N>>
  >;
// clang-format on

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
void run_rule110(std::integer_sequence<std::size_t, levels...>)
{
    std::vector<std::pair<int, char>> v;

    ((v.clear(), tur::util::unpurify(v, typename Rule110<Tape, levels>::Tape{}), rule110_helper(v)), ...);
}

template <class List, class Letter, std::size_t offset, size_t level, std::size_t... levels>
struct AppendTapeCells {
    using Tape =
      AppendTapeCells<tl::AppendList<tur::TapeCell<offset + level, Letter>, List>, Letter, offset, levels...>::Tape;
};

template <class List, class Letter, std::size_t offset, size_t level>
struct AppendTapeCells<List, Letter, offset, level> {
    using Tape = tl::AppendList<tur::TapeCell<offset + level, Letter>, List>;
};

template <class List, class T, std::size_t offset, std::size_t... levels>
auto append_tape_cells(std::integer_sequence<std::size_t, levels...>)
  -> AppendTapeCells<List, T, offset, levels...>::Tape;

template <std::size_t width>
using BaseTape = tl::List<tur::TapeCell<0, START>, tur::TapeCell<width, LET1>, tur::TapeCell<width + 1, END>>;

// Constructs the tape to run rule110 simulations on.
// The contents are as follows:
// START, LET0, LET0, ..., LET0, LET1, END
// START and END are not included into width
template <std::size_t width>
auto make_tape() -> decltype(append_tape_cells<BaseTape<width>, LET0, 1>(std::make_index_sequence<width - 1>()));

int main()
{
    constexpr std::size_t WIDTH = 10;
    constexpr std::size_t ITERATIONS = 10;

    using TapeList = decltype(make_tape<WIDTH>());
    run_rule110<TapeList>(std::make_index_sequence<ITERATIONS>{});

    return 0;
}
