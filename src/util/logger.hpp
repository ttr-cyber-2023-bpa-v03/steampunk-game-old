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

namespace util {
    namespace macro_helpers {
        // Forward declaration
        struct trace;
    }

    enum class log_level : int {
        debug,
        info,
        warning,
        error,
        fatal
    };

    // In an attempt to prevent bulk when using the logger, I made a bunch of static
    // functions that access the singleton and
    class log : public std::enable_shared_from_this<log> {
        // Log path (if writing to a file)
        std::string _log_path = "";

        // Stream to the log.
        std::unique_ptr<std::ostream> _log;

        // Ensures that only one thread can write to the log file at a time.
        std::mutex _mutex{};

        // Writes a message to the log.
        void write_log(const std::string& message);

    public:
        static inline constexpr std::string_view level_to_string(const log_level log_level) {
            switch (log_level) {
                case log_level::debug:
                    return "DEBUG";
                case log_level::info:
                    return "INFO";
                case log_level::warning:
                    return "WARNING";
                case log_level::error:
                    return "ERROR";
                case log_level::fatal:
                    return "FATAL";
                default:
                    return "???";
            }
        }

        // Returns the singleton instance of the world
        static std::shared_ptr<log>& instance();

        // Initialize the logger.
        static void init(bool console = false);

        // Returns a shared pointer to this class.
        std::shared_ptr<log> ref() {
            return shared_from_this();
        }

        log(bool console);

        // Send a generic message to the log.
        static void send(const log_level log_level, const std::string& message);

        // Send a generic message to the log with formatting.
        template<typename... Args>
        static void send(
            const log_level log_level,
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
} // namespace util

namespace util::macro_helpers {
    struct trace {
        std::string content;
    };

    // Generates a trace string from the function name, file name, and line number.
    inline trace generate_trace(
        const std::string func_name, 
        const std::string_view file_path,
        const int line
    ) {
        return { func_name + " @ " + std::string(file_path) + ':'
            + std::to_string(line) };
    }
} // namespace util::macro_helpers

// Used to generate a static trace of where a log message came from.
// Example: SG_TRACE => "void game::world::start() @ 42:world.cpp"
#define SG_TRACE ::util::macro_helpers::generate_trace( \
    __func__,                                           \
    __FILE__,                                           \
    __LINE__                                            \
)

