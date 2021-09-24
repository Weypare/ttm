#ifndef TM_HPP
#define TM_HPP

#include "type_list.hpp"

namespace turing_machine
{
    namespace tl = type_list;

    enum class Direction { L = -1, N = 0, R = 1 };
    struct Nil;

    inline namespace tape
    {
        using position_t = int;
        static_assert(std::is_signed_v<position_t>, "Position type must be signed");

        struct Blank {
            static constexpr auto value = 'B';
        };

        template <position_t pos, class Letter = Blank>
        struct TapeCell {
            static constexpr auto position = pos;
            using letter = Letter;
        };
    } // namespace tape

    inline namespace state
    {
        template <typename State, class Letter>
        struct TransitionFrom {
            using state = State;
            using letter = Letter;
        };
        template <class State, class Letter, Direction Dir>
        struct TransitionTo {
            using state = State;
            using letter = Letter;
            static constexpr auto dir = Dir;
        };

        template <class From, class To>
        struct Transition {
            using from = From;
            using to = To;
        };

    } // namespace state

    namespace detail
    {
        template <position_t pos, class State, class FinalStates, class TransitionTable, class Tape>
        struct MachineImpl {
            using state = State;
            using final_states = FinalStates;
            using transition_table = TransitionTable;
            using tape = std::conditional_t<std::is_same_v<Tape, tl::EmptyList>, tl::List<TapeCell<0, Blank>>, Tape>;
            static constexpr auto position = pos;
        };
    } // namespace detail

    template <class InitState, class FinalStates, class TransitionTable, class TapeList>
    using Machine = detail::MachineImpl<0, InitState, FinalStates, TransitionTable, TapeList>;

    namespace detail
    {
        template <class From, class TransitionTable>
        struct ContainsTransition;
        template <class From>
        struct ContainsTransition<From, tl::EmptyList> {
            static constexpr auto value = false;
        };
        template <class From, class TransitionTable>
        struct ContainsTransition {
            static constexpr auto value =
              std::is_same_v<
                From,
                typename tl::Head<TransitionTable>::from> || ContainsTransition<From, tl::Tail<TransitionTable>>::value;
        };

        template <class From, class TransitionTable>
        struct FindTransition {
            using head = tl::Head<TransitionTable>;
            using tail = tl::Tail<TransitionTable>;
            using transition = typename std::conditional_t<std::is_same_v<From, typename head::from>,
                                                           typename head::to,
                                                           typename FindTransition<From, tail>::transition>;
        };
        template <class From>
        struct FindTransition<From, tl::EmptyList> {
            using transition = Nil;
        };

        template <position_t pos, class Tape>
        struct ContainsTapePos;
        template <position_t pos>
        struct ContainsTapePos<pos, tl::EmptyList> {
            static constexpr auto value = false;
        };
        template <position_t pos, class Tape>
        struct ContainsTapePos {
            static constexpr auto value =
              (tl::Head<Tape>::position == pos) || ContainsTapePos<pos, tl::Tail<Tape>>::value;
        };

        template <position_t pos, class Tape>
        struct FindTapeCell;

        template <position_t pos, class Tape>
        struct FindTapeCell {
            using cell = std::conditional_t<tl::Head<Tape>::position == pos,
                                            tl::Head<Tape>,
                                            typename FindTapeCell<pos, tl::Tail<Tape>>::cell>;
        };
        template <position_t pos>
        struct FindTapeCell<pos, tl::EmptyList> {
            using cell = Nil;
        };

        template <class Machine>
        struct Step {
            using CurrentTapeCell = typename FindTapeCell<Machine::position, typename Machine::tape>::cell;

            static_assert(ContainsTransition<TransitionFrom<typename Machine::state, typename CurrentTapeCell::letter>,
                                             typename Machine::transition_table>::value,
                          "Missing transition. Check your machine configuration.");
            using Transition =
              typename FindTransition<TransitionFrom<typename Machine::state, typename CurrentTapeCell::letter>,
                                      typename Machine::transition_table>::transition;

            static constexpr auto NewPos = Machine::position + static_cast<position_t>(Transition::dir);
            using NewTape_ = std::conditional_t<ContainsTapePos<NewPos, typename Machine::tape>::value,
                                                typename Machine::tape,
                                                tl::AppendList<TapeCell<NewPos>, typename Machine::tape>>;
            using NewTape =
              tl::Replace<CurrentTapeCell, TapeCell<CurrentTapeCell::position, typename Transition::letter>, NewTape_>;
            using NewMachine = MachineImpl<NewPos,
                                           typename Transition::state,
                                           typename Machine::final_states,
                                           typename Machine::transition_table,
                                           NewTape>;
        };

        template <class Machine, bool finished>
        struct RunImpl;

        template <class Machine>
        struct RunImpl<Machine, true> {
            using state = typename Machine::state;
            using tape = typename Machine::tape;
            static constexpr auto position = Machine::position;
        };

        template <class Machine>
        struct RunImpl<Machine, false> {
            using NextMachine = typename Step<Machine>::NewMachine;
            static constexpr auto finished =
              tl::Contains<typename NextMachine::state, typename NextMachine::final_states>;
            using state = typename RunImpl<NextMachine, finished>::state;
            using tape = typename RunImpl<NextMachine, finished>::tape;
            static constexpr auto position = RunImpl<NextMachine, finished>::position;
        };
    } // namespace detail

    template <class Machine>
    using Run = detail::RunImpl<Machine, false>;
} // namespace turing_machine

#endif
