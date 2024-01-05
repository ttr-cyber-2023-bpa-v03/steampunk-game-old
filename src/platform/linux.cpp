#if defined (__linux__)

#include "game/world.hpp"
#include "sched/runner.hpp"

#include "linux.hpp"
#include <sched.h>
#include <csignal>
#include <unordered_map>

namespace platform {
    // Convert an affinity mask into a cpu_set_t structure which is what Linux
    // understands internally
    void affinity_mask_to_cpu_set(const affinity_mask mask, cpu_set_t& set) {
        CPU_ZERO(&set);
        for (int i = 0; i < sizeof(affinity_mask) * 8; i++) {
            if (mask & (1 << i))
                CPU_SET(i, &set);
        }
    }

    void set_thread_affinity(std::thread& thread, const affinity_mask mask) {
#   warning "Thread affinity is not implemented on Linux. Game may have reduced performance."
        return;
    }

    struct signal_dispatcher {
        static std::unordered_map<int, signal_handler> callbacks;

        // This code will overwrite other handlers if they exist
        static void handle(int sig, signal_handler&& callback) {
            callbacks[sig] = std::move(callback);
            std::signal(sig, [](int signal) {
                std::cout << "signal arrived " << signal << std::endl;
                callbacks[signal](signal);
            });
        }
    };

    std::unordered_map<int, signal_handler> signal_dispatcher::callbacks;

    void on_close(signal_handler callback) {
        signal_dispatcher::handle(SIGINT, std::move(callback));
        signal_dispatcher::handle(SIGTERM, std::move(callback));
    }
}

#endif