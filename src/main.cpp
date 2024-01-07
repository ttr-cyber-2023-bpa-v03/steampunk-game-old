#include "SDL.h"
#include "SDL_messagebox.h"
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

#include "logging/logger.hpp"
#include "logging/macros.hpp"
#include <stdexcept>
using logging::logger;

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

void exception_filter() {
	try {
		// Propagate the exception to a try/catch block
        std::rethrow_exception(std::current_exception());
	}
	catch (std::exception& ex) {
		// we l0g here
		logger::send(logger::level::fatal, SG_TRACE, "Unhandled exception: ", ex.what());
    }
	catch (...) {
		// this is equivalent to "i wanna die"
		logger::send(logger::level::fatal, "Unhandled exception: wow there is no exception");
	}

	// This is the desired "abnormal" exit point of the program, where we just dump the
	// memory and call it a day. The user can then decide if they want to report the
	// issue, ignore it, or dig into it themselves and make a PR (please im begging you).

	const std::string msg = "An unhandled exception has occurred. The program will now exit.";
	const std::string question = "Would you like to report this via email?";

	// Make sure we note this in the log
	logger::send(logger::level::fatal, msg);

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
		logger::send(logger::level::fatal, "SDL_ShowMessageBox failed: ", SDL_GetError());
	}

	// The user wnats to report the issue
	if (selected_button == 0) {
		// Damn... alright so we need to get the log path
		const auto log_path = logger::log_path();
		if (log_path.has_value()) {
			// We can construct a mailto link with this path as the attachment
			std::stringstream ss{};

			// Add the email address (meh for now just take my email)
			// Crude? Yes. Effective? Kinda. I'll fix later its 1 in the morning.
			ss << "mailto";
			ss << ":realnickk1@gmail.com";
			ss << "?subject=steampunk-game%20-%20Crash%20Report";
			ss << "&body=Please%20describe%20what%20you%20were%20doing%20when%20the%20crash%20occurred.%0D%0A";
			ss << "Please%20attach%20the%20log%20file%20at%20" << log_path.value() << "%0D%0A";
			ss << "We%20appreciate%20your%20help%20in%20making%20this%20game%20better!";

			// Open the link in the default browser
			platform::open_url(ss.str());
		}
		else {
			// Ok you're writing logs to the console. I guess we can't do anything about
			// that. If you are writing logs to the console then you should already know
			// what you're doing because you got a custom build of the game.
			logger::send(
				logger::level::fatal, 
				"Cannot send crash report: Log path is not set."
			);
		}
	}

	platform::dump_and_exit();
}

int main(int argc, char* argv[]) {
	// Initialize our amazing logging system.
	logger::init(false);

	// Set up unhandled exception handling. A crash is not acceptable!
	std::set_terminate(exception_filter);
	throw std::runtime_error("test");

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
	auto text = std::make_shared<rendering::text_box>();
	text->set_font("Terminus.ttf", 64);
	text->set_text("usb dongle simulator 0.1");
	text->position = { 0, 0 };
	world->render_job->add_renderable(text);

	// Start the scheduler, which will also block this thread until the scheduler is
	// gracefully stopped
	world->set_fps(144);
	world->start();

	// This is the desired "normal" exit point of the program.
	return 0;
}
