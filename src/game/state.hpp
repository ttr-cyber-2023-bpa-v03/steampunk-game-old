#pragma once

#include <memory>

namespace sched {
    // Forward declarations
    class runner;
}

namespace game {
    // Forward declarations
    class object;
    class write_job;

    struct state {
        static std::shared_ptr<sched::runner> scheduler;
        static std::shared_ptr<object> root;
        static std::shared_ptr<write_job> j_write;
    };
}