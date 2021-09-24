# Template Turing Machine

Turing Machine simulator using C++ templates.

In order to initialize the machine you have to specify:
- initial state
- list of final states
- state transition table
- initial tape contents

Final states, transition table and tape contents must be template specializations of [`List`](include/type_list.hpp).
If machine cursor moves to a tape cell which was not specified in initial tape contents list, the tape cell will be
lazily initialized with a [`Blank`](include/tm.hpp) letter.

To run the machine, use [`Run`](include/tm.hpp) template. If your code compiles, it means that the machine was executed
successfully and you can access the final machine state, tape contents, and cursor position in `Run::state`, `Run::tape`
and `Run::position` respectively.

To get tape cells after a successful run, you can use [`unpurify`](include/util.hpp) to dump tape contents to any
container which provides `push_back` interface and specifies its `value_type`.

## Usage

Example of the machine, which replaces all letters `1` on tape with `2`:

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

#include "type_list.hpp"
#include "tm.hpp"
#include "util.hpp"

namespace tl = type_list;
namespace tur = turing_machine;

int main() {
    using S1 = tur::util::CharState<'1'>;
    using S2 = tur::util::CharState<'2'>;

    using Blank = tur::Blank;
    using Let1 = tur::util::CharLetter<'1'>;
    using Let2 = tur::util::CharLetter<'2'>;

    using TapeList =
      tl::List<tur::TapeCell<0, Let1>, tur::TapeCell<1, Let2>, tur::TapeCell<2, Let1>>;

    using StartState = S1;
    using FinalStates = tl::List<S2>;
    using TransitionTable =
      tl::List<tur::Transition<tur::TransitionFrom<S1, Let1>,  tur::TransitionTo<S1, Let2,  tur::Direction::R>>,
               tur::Transition<tur::TransitionFrom<S1, Let2>,  tur::TransitionTo<S1, Let2,  tur::Direction::R>>,
               tur::Transition<tur::TransitionFrom<S1, Blank>, tur::TransitionTo<S2, Blank, tur::Direction::N>>
               >;

    using ReplaceMachine = tur::Machine<StartState, FinalStates, TransitionTable, TapeList>;
    using Final = tur::Run<ReplaceMachine>;
    static_assert(tl::Contains<Final::state, FinalStates>, "Turing Machine is not in accepting state");

    std::vector<std::pair<int, char>> v;

    tur::util::unpurify(v, Final::tape{});
    std::sort(v.begin(), v.end(), [](const auto &a, const auto &b) {
        return a.first < b.first;
    });

    for (const auto &el : v) {
        std::cout << el.first << ": " << el.second << std::endl;
    }
    // Output:
    // 0: 2
    // 1: 2
    // 2: 2
    // 3: B

    return 0;
}
```
