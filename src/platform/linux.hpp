#pragma once

#if defined(__linux__)

#include <thread>
#include <cstdint>

namespace platform {
    // Size of word
    using affinity_mask = std::uintptr_t;

    void set_thread_affinity(std::thread& thread, const affinity_mask mask);
}

#else

#warning "Included Linux system abstraction layer on a non-linux platform. This is probably a mistake."

#endif