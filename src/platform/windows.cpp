#if defined(_WIN32)

#include "windows.hpp"

#include <stdexcept>

namespace platform {
    void set_thread_affinity(std::thread& thread, const affinity_mask mask) {
        if (!SetThreadAffinityMask(thread.native_handle(), mask))
            throw std::runtime_error("Failed to set thread affinity");
    }

    void close_on_term() {
        // no-op
    }

    std::string executable_path() {
        std::string path_str(MAX_PATH, '\0');
        const auto path_len = GetModuleFileNameA(nullptr, path_str.data(), MAX_PATH);

        if (path_len == 0)
            throw std::runtime_error("GetModuleFileNameA failed");
        path_str.resize(path_len);

        return path_str;
    }
}

#endif