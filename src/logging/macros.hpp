#pragma once

#include <string>
#include <filesystem>

namespace logging::macro_helpers {
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
}

// Used to generate a static trace of where a log message came from.
// Example: SG_TRACE => "void game::world::start() @ 42:world.cpp"
#define SG_TRACE ::logging::macro_helpers::generate_trace( \
    __func__,                                              \
    __FILE__,                                              \
    __LINE__                                               \
)
