#include "game/state.hpp"
#include "game/write_job.hpp"

#include "sched/runner.hpp"
#include "sched/worker.hpp"

#include "util/debug.hpp"

#include "rendering/render_job.hpp"
#include <iostream>

// Alias for the state class. Saves me from a migraine.
using game::state;

class test_object : public game::object {
public:
	test_object() : game::object("test_object") {
		SG_LOCATE;
	}
};

int main(int argc, char* argv[]) {
	{
		// Initialize the scheduler
		state::scheduler = std::make_shared<sched::runner>();
		state::scheduler->frame_delay = 1.0L / 60.0; // 60 fps default

		// Initialize the write job
		state::j_write = std::make_shared<game::write_job>();
		state::scheduler->schedule(state::j_write);

		// Initialize the renderer
		//const auto render_job = std::make_shared<rendering::render_job>("Viewport", glm::vec2{ 800, 600 });
		//state::scheduler->schedule(render_job);

		// we test stuff hehe
		SG_LOCATE;
		test_object obj{};
		test_object obj2{};

		obj.add_child(obj2);

		// Start the scheduler arbiter
		state::scheduler->start(false);
	}

	return 0;
}
