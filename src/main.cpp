#include "game/world.hpp"
#include "game/write_job.hpp"

#include "platform/linux.hpp"
#include "sched/runner.hpp"
#include "sched/worker.hpp"

#include "util/debug.hpp"

#include "rendering/render_job.hpp"

#include "platform/current.hpp"

#include <csignal>
#include <iostream>
#include <memory>

class test_object : public game::object {
public:
	test_object() : game::object("test_object") {
		SG_LOCATE;
	}
};

int main(int argc, char* argv[]) {
	// Initialize the world
	auto& world = game::world::instance();

	// Create a test object
	auto test = std::make_shared<test_object>();

	world->add_child(test);

	return 0;
}
