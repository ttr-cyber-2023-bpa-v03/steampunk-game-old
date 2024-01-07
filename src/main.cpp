#include "SDL.h"
#include "SDL_messagebox.h"
#include "game/world.hpp"
#include "game/write_job.hpp"

#include "platform/linux.hpp"
#include "sched/job.hpp"
#include "sched/runner.hpp"
#include "sched/worker.hpp"

#include "rendering/render_job.hpp"
#include "rendering/renderables/text_box.hpp"

#include "platform/current.hpp"

#include "logging/logger.hpp"
#include "logging/macros.hpp"
#include "util/uri_tools.hpp"
#include <stdexcept>
using logging::logger;

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
	try {
		// Propagate the exception to a try/catch block
        std::rethrow_exception(std::current_exception());
	}
	catch (std::exception& ex) {
		// we l0g here
		logger::send(SG_TRACE, "Unhandled exception: ", ex.what());
    }
	catch (...) {
		// this is equivalent to "i wanna die"
		logger::send(logging::level::fatal, "Unhandled exception: wow there is no exception");
	}

	// This is the desired "abnormal" exit point of the program, where we just dump the
	// memory and call it a day. The user can then decide if they want to report the
	// issue, ignore it, or dig into it themselves and make a PR (please im begging you).

	const std::string msg = "An unhandled exception has occurred. The program will now exit.";
	const std::string question = "Would you like to report this via email?";

	// Make sure we note this in the log
	logger::send(logging::level::fatal, msg);

	// Show a message box so the app doesn't just disappear into the void
	const std::string mb_text = msg + "\n" + question;

	const SDL_MessageBoxButtonData buttons[] = {
		{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Yes" },
		{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "No" },
	};

    SDL_MessageBoxData mb{
		SDL_MESSAGEBOX_ERROR,
		nullptr,
		"Fatal Error",
		mb_text.c_str(),
		sizeof(buttons) / sizeof(buttons[0]), // number of buttons
		buttons,
		nullptr,
	};

	int selected_button = 1; // default to 'no'
	if (SDL_ShowMessageBox(&mb, &selected_button) < 0) {
		// Well... we tried and failed. Obviously SDL hasn't initialized.
		logger::send(logging::level::error, "SDL_ShowMessageBox failed: ", SDL_GetError());
	}

	// The user wnats to report the issue
	if (selected_button == 0) {
		// Damn... alright so we need to get the log path
		const auto log_path = logger::log_path();
		if (log_path.has_value()) {
			// We can construct a mailto link with this path as the attachment
			util::mailto mail{ 
				"realnickk1@gmail.com", 
				"steampunk-game - Crash Report",
				"Please describe what you were doing when the crash occurred:\n\nIf you can, please attach the log file to this email, which is located at " + log_path.value() + "."
			};
			mail.open();
		}
		else {
			// Ok you're writing logs to the console. I guess we can't do anything about
			// that. If you are writing logs to the console then you should already know
			// what you're doing because you got a custom build of the game.
			logger::send(
				logging::level::error, 
				"Cannot send crash report: Log path is not set."
			);
		}
	}

	platform::dump_and_exit();
}

int main(int argc, char* argv[]) {
	// Initialize our amazing logging system.
	logger::init(true);

	// Set up unhandled exception handling. A crash is not acceptable!
	std::set_terminate(exception_filter);

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

	// Start the scheduler, which will also block this thread until the scheduler is
	// gracefully stopped
	world->set_fps(144);
	world->start();

	// This is the desired "normal" exit point of the program.
	return 0;
}
