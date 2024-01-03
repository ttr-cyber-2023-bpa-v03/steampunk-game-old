#if defined(_WIN32)

#include "windows.hpp"

#include <stdexcept>

namespace platform {
    void set_thread_affinity(std::thread& thread, const affinity_mask mask) {
        if (!SetThreadAffinityMask(thread.native_handle(), mask))
            throw std::runtime_error("Failed to set thread affinity");
    }
}

#endif