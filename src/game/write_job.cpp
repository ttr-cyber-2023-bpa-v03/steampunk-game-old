#include "write_job.hpp"
#include "object.hpp"

namespace game {
    void write_job::execute() {
        // Acquire a write lock to ensure no other threads can
        // read or write to the object properties and cause a
        // data race. Single lock is done here for efficiency
        // (concurrent R/W is rare in this case).
        std::lock_guard lock(mutex);

        // Empty the queue and execute all pending functions
        std::unique_ptr<std::function<void()>> pfn;
        while (queue.try_pop(pfn)) {
            (*pfn)();
            // RAII should take care of the rest
        }
    }

    void write_job::enqueue(std::function<void()>&& fn) {
        queue.push(std::make_unique<std::function<void()>>(std::move(fn)));
    }
}
