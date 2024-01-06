#include "world.hpp"
#include "object.hpp"
#include "write_job.hpp"

#include "sched/runner.hpp"
#include "sched/worker.hpp"
#include "rendering/render_job.hpp"
#include <cerrno>

namespace game {
    std::shared_ptr<world>& world::instance() {
        static std::once_flag flag;
        static std::shared_ptr<world> instance;

        std::call_once(flag, [] { instance = std::make_shared<world>(); });
        return instance;
    }

    world::world() : object("world") {
        // Initialize the scheduler
        scheduler = std::make_shared<sched::runner>();

        // Initialize the write job and schedule it
        write_job = std::make_shared<game::write_job>();
        scheduler->schedule(write_job);

        // Initialize the renderer
        render_job = std::make_shared<rendering::render_job>("Viewport", glm::vec2{800, 600 });
        write_job->schedule(render_job);
    }

    void world::set_fps(int fps) {
        // Note that no synchronization has do be done here because this field
        // is atomic
        scheduler->frame_delay = 1.0 / fps;
    }

    int world::get_fps() {
        return static_cast<int>(1.0 / scheduler->frame_delay);
    }

    void world::start(bool floating) {
        scheduler->start(floating);
    }

    void world::stop(bool signal_only) {
        if (signal_only)
            scheduler->signal_stop();
        else
            scheduler->stop();
    }
}