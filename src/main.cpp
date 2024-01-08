#include "SDL.h"
#include "SDL_messagebox.h"
#include "game/world.hpp"
#include "game/write_job.hpp"

#include "sched/job.hpp"
#include "sched/runner.hpp"
#include "sched/worker.hpp"

#include "rendering/render_job.hpp"
#include "rendering/renderables/text_box.hpp"

#include "platform/current.hpp"

#include "util/uri_tools.hpp"
#include "util/logger.hpp"

#include <stdexcept>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>

class fps_reporter final : public sched::job {
	std::shared_ptr<rendering::text_box> fps_box;

public:
	fps_reporter() {
		fps_box = std::make_shared<rendering::text_box>();
		fps_box->set_text("FPS: 0");
		fps_box->set_font({ "Terminus.ttf", 32 });
		fps_box->set_position({ 16, 16 });
		fps_box->set_color({ 255, 255, 255, 255 });
		game::world::instance()->render_job->add_renderable(fps_box);
	}

	void execute() override {
		auto world = game::world::instance();
		fps_box->set_text( "FPS: " + std::to_string((int)(1.0 / world->scheduler->cycle_delta)));
	}
};

void exception_filter() {
	const std::string msg = "An unhandled exception has occurred. The program will now exit.";
	const std::string question = "Would you like to report this via email?";

	// Make sure we note the main message in the log
	util::log::send(util::log_level::fatal, msg);

	// Determine what the exception is
	try {
		// Propagate the exception to a try/catch block
        std::rethrow_exception(std::current_exception());
	}
	catch (std::exception& ex) {
		// Log what the exception says
		util::log::send(util::log_level::error, "Unhandled exception: {}", ex.what());
    }
	catch (...) {
		// If anything gets thrown that isn't an exception, we will just log it
		util::log::send(util::log_level::error, "Unhandled exception: Unknown");
	}

	// Show a message box so the app doesn't just disappear into the void, defaulting to
	// 'No' if the message box fails to show
	auto next_action = 1; // mb_btns[1]
	{
		const std::string mb_text = msg + "\n" + question;

		const SDL_MessageBoxButtonData mb_btns[] = {
			{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Yes" },
			{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "No" },
		};

		SDL_MessageBoxData mb{
			SDL_MESSAGEBOX_ERROR,
			nullptr,
			"Fatal Error",
			mb_text.c_str(),
			sizeof(mb_btns) / sizeof(mb_btns[0]), // number of buttons
			mb_btns,
			nullptr,
		};

		// If the message box fails to show, we will just log and continue
		if (SDL_ShowMessageBox(&mb, &next_action) < 0)
			util::log::send(util::log_level::error, "SDL_ShowMessageBox failed: ", SDL_GetError());
	}

	// The user wants to report the issue. We will construct a mailto link and open it
	if (next_action == 0) {
		// Construct the body of the email
		std::stringstream body{};
		body << "Please describe what you were doing when the crash occurred:\n\n";

		auto log_path = util::log::log_path();
		if (log_path.has_value()) {
			// There is a log file, but we can't attach it using a mailto link from the
			// research I have done.
			body << "Please attach the log file to this email. It is located at " << log_path.value();
		}

		// Construct the mailto link and open it
		util::mailto mail{ 
			"realnickk1@gmail.com", // Just going to use my dev email for now
			"steampunk-game - Crash Report",
			body.str()
		};
		platform::open_url(mail.to_string());
	}

	// Dump the memory and exit. Ripperoni.
	platform::dump_and_exit();
}

int main(int argc, char* argv[]) {
	// This will handle sneaky exceptions.
	std::set_terminate(exception_filter);

	// Initialize the log system, and write to console if we are in debug mode
	util::log::init(true);

	// Initialize the world
	auto world = game::world::instance();

	world->key_down->connect([](char key) {
		util::log::send(util::log_level::info, "Key pressed: {}", key);
	});

	// FPS reporting (this is primarily for debugging)
	world->scheduler->schedule(std::make_shared<fps_reporter>());

	platform::on_close([world](int signal) {
		// Signal a scheduler stop and hope for the best
		// 9/10 times this will work perfectly fine but if you have a long running job
		// it will cause a hang
		world->stop();
	});

	// Start the scheduler, which will also block this thread until the scheduler is
	// gracefully stopped
	world->set_fps(0);
	world->start();

	// This is the desired "normal" exit point of the program.
	SDL_Quit();
	return 0;
}
