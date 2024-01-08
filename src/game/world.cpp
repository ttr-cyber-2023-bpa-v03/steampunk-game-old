#include "world.hpp"
#include "SDL.h"
#include "SDL_video.h"
#include "object.hpp"
#include "util/logger.hpp"
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
        // Note that no synchronization has do be done here because frame_delay is atomic

        // Match refresh rate of monitor if fps is 0
        // Also prevents division by zero
        if (fps == 0) {
            // Check video init in SDL and initialize if not
            // This is required to get the refresh rate of the monitor
            const auto video_init = SDL_WasInit(SDL_INIT_VIDEO) != 0;
            if (!video_init)
                SDL_Init(SDL_INIT_VIDEO);

            SDL_DisplayMode display;
            if (SDL_GetDesktopDisplayMode(0, &display) < 0) {
                // SDL failed to get the display mode, so we will default to 60fps
                // Not a big deal, but we should log this for debugging purposes
                util::log::send(util::log_level::warning, "Failed to get monitor refresh rate: {}. Defaulting to 60Hz.", SDL_GetError());
                scheduler->frame_delay = 1.0L / 60;
            }
            else {
                // We will use the refresh rate of the monitor
                scheduler->frame_delay = 1.0L / display.refresh_rate;
            }

            return;
        }
        
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