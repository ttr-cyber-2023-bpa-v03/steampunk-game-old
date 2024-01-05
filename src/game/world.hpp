#pragma once

#include <memory>

#include "object.hpp"

// Forward declarations
namespace sched {
    class runner;
}

// Forward declarations
namespace rendering {
    class render_job;
}

namespace game {
    // Forward declarations
    class write_job;

    // The root object of the entire game
    class world : public object {
    public:
        std::shared_ptr<sched::runner> scheduler;

        std::shared_ptr<game::write_job> write_job;

        std::shared_ptr<rendering::render_job> render_job;

        // Returns the singleton instance of the world
        static std::shared_ptr<world>& instance();

        world();

        // Get the maximum framerate of the game's scheduler
        int get_fps();

        // Set the maximum framerate of the game's scheduler
        void set_fps(int fps);
    };
}