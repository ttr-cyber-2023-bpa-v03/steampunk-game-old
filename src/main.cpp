#include "game/world.hpp"
#include "game/write_job.hpp"

#include "platform/linux.hpp"
#include "sched/job.hpp"
#include "sched/runner.hpp"
#include "sched/worker.hpp"

#include "util/debug.hpp"

#include "rendering/render_job.hpp"
#include "rendering/renderables/text_box.hpp"

#include "platform/current.hpp"

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>

class fps_reporter final : public sched::job {
public:
	void execute() override {
		auto world = game::world::instance();
		std::cout << "FPS: " << (int)(1.0 / world->scheduler->cycle_delta) << std::endl;
	}
};

int main(int argc, char* argv[]) {
	// Initialize the world
	auto world = game::world::instance();

	// FPS reporting (this is primarily for debugging)
	world->scheduler->schedule(std::make_shared<fps_reporter>());

	platform::on_close([world](int signal) {
		// Signal a scheduler stop and hope for the best
		// 9/10 times this will work perfectly fine but if you have a long running job
		// it will cause a hang
		world->stop();
	});

	// Create a test object
	/*auto text = std::make_shared<rendering::text_box>();
	text->set_font("Terminus.ttf", 64);
	text->set_text("orangutan");
	text->position = { 0, 0 };
	world->render_job->add_renderable(text);*/

	// Start the scheduler, which will also block this thread until the scheduler is
	// gracefully stopped
	world->set_fps(1000000000);
	world->start();

	// This is the desired "normal" exit point of the program.
	return 0;
}
