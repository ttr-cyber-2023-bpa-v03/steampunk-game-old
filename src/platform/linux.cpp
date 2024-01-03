#if defined (__linux__)

#include "linux.hpp"
#include <sched.h>

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
}

#endif