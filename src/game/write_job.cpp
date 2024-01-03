#include "write_job.hpp"
#include "object.hpp"

namespace game {
    void write_job::execute() {
        // Acquire a write lock to ensure no other threads can
        // read or write to the object properties and cause a
        // data race. Single lock is done here for efficiency
        // (concurrent R/W is rare in this case).
        std::lock_guard lock(mutex);

        // Empty the queue and write all pending values
        std::unique_ptr<req_write> req;
        while (_rw_queue.try_pop(req)) {
            req->obj->set_unsafe(req->property, req->value);
        }
    }

    void write_job::enqueue_write(const std::shared_ptr<object>& obj, const std::string& property, std::shared_ptr<void>& value) {
        _rw_queue.push(std::make_unique<req_write>(obj, property, value));
    }
}
