#include "logger.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include "platform/current.hpp"

namespace util {
    namespace fs = std::filesystem;

    static std::shared_ptr<log> logger_instance{};

    void log::init(bool console) {
        logger_instance = std::make_shared<log>(console);
    }

    log::log(bool console) {
        if (console) {
            // Redirect the log to the console
            _log = std::make_unique<std::ostream>(std::cout.rdbuf());
        }
        else {
            // Get executable directory
            const auto exec_path = platform::executable_path();
            const auto exec_dir = fs::path{ exec_path }.parent_path();

            // Get log file path
            _log_path = (exec_dir / "log.txt").generic_string();
            if (fs::exists(_log_path)) {
                // If the log file already exists, delete it
                fs::remove(_log_path);
            }

            // Create the log file
            _log = std::make_unique<std::ofstream>(_log_path, std::ios::out | std::ios::app);
        }
    }

    void log::write_log(const std::string& message) {
        // Handle non-init. Write to console as a last ditch effort.
        if (logger_instance == nullptr) {
            std::cerr << "[NON-INIT] " << message << std::endl;
            return;
        }

        std::lock_guard<std::mutex> lock{ _mutex };
        *_log << message << std::endl;
    }

    void log::send(const log_level log_level, const std::string& message) {
        std::stringstream ss;

        // Thread ID, good for debugging scheduler jobs
        ss << "[" << std::this_thread::get_id() << "] ";

        // Timestamp, because why not
        ss << "[" << std::format("{0:%T}", std::chrono::system_clock::now())
           << "] ";

        // Log level, good for searching for specific messages
        ss << "[" << level_to_string(log_level) << "] ";

        // The message, because logging nothing won't help anyone
        ss << message;

        // Write the message to the log
        logger_instance->write_log(ss.str());
    }

    void log::send(
        const macro_helpers::trace&& trace,
        const std::string& message
    ) {
        send(log_level::debug, std::format("{}: {}", trace.content, message));
    }

    std::optional<std::string> log::log_path() {
        // Handle non-init
        if (logger_instance == nullptr) 
            return std::nullopt;

        auto log_path = logger_instance->_log_path;
        return log_path == "" ? std::nullopt : std::make_optional(log_path);
    }
}