#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <ostream>
#include <mutex>
#include <memory>
#include <sstream>
#include <string>
#include <thread>

namespace logging {
    namespace macro_helpers {
        // Forward declaration
        struct trace;
    }

    enum class level : int {
        debug,
        info,
        warning,
        error,
        fatal
    };

    // In an attempt to prevent bulk when using the logger, I made a bunch of static
    // functions that access the singleton and
    class logger : public std::enable_shared_from_this<logger> {
        // Log path (if writing to a file)
        std::string _log_path = "";

        // Stream to the log.
        std::unique_ptr<std::ostream> _log;

        // Ensures that only one thread can write to the log file at a time.
        std::mutex _mutex{};

        // Writes a message to the log.
        void write_log(const std::string& message);

    public:
        static inline constexpr std::string_view level_to_string(const level log_level) {
            switch (log_level) {
                case level::debug:
                    return "DEBUG";
                case level::info:
                    return "INFO";
                case level::warning:
                    return "WARNING";
                case level::error:
                    return "ERROR";
                case level::fatal:
                    return "FATAL";
                default:
                    return "???";
            }
        }

        // Returns the singleton instance of the world
        static std::shared_ptr<logger>& instance();

        // Initialize the logger.
        static void init(bool console = false);

        // Returns a shared pointer to this class.
        std::shared_ptr<logger> ref() {
            return shared_from_this();
        }

        logger(bool console);

        // Send a generic message to the log.
        static void send(const level log_level, const std::string& message);

        // Send a generic message to the log with formatting.
        template<typename... Args>
        static void send(
            const level log_level,
            const std::string_view message,
            Args&&... args
        ) {
            send(log_level, std::vformat(message, std::make_format_args(args...)));
        }

        // Intended for use with SG_TRACE
        static void send(const macro_helpers::trace&& trace,
                         const std::string &message);

        // Intended for use with SG_TRACE
        template<typename... Args>
        static void send(
            const macro_helpers::trace&& trace,
            const std::string_view message,
            Args&&... args
        ) {
            send(std::move(trace), std::vformat(message, std::make_format_args(args...)));
        }

        // Get the log path
        [[nodiscard]] static std::optional<std::string> log_path();
    };
}