#pragma once

#if defined(__linux__)

#include <functional>
#include <thread>
#include <cstdint>

namespace platform {
    // Size of word
    using affinity_mask = std::uintptr_t;

    void set_thread_affinity(std::thread& thread, const affinity_mask mask);

    using signal_handler = std::function<void(int)>;
    void on_close(signal_handler callback);

    std::string executable_path();

    void dump_and_exit();

    void open_url(const std::string& url);
}

#else

#pragma message ("Included Linux system abstraction layer on a non-linux platform. This is probably a mistake.")

#endif