#include "state.hpp"
#include "game/write_job.hpp"

namespace game {
    // Once upon a time I forgot to write 'state::' and it took me
    // 2 hours to figure out why the linker was complaining about
    // an unresolved external symbol. I'm not making that mistake
    // again, and I am aligning my code to remind me to write it.

    std::shared_ptr<sched::runner> state::scheduler = nullptr;
    std::shared_ptr<object>        state::root      = nullptr;
    std::shared_ptr<write_job>     state::j_write   = nullptr;
}