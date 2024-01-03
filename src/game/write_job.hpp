#pragma once

#include <tbb/concurrent_queue.h>

#include "sched/job.hpp"

namespace game {
    class object;
    
    class write_job final : public sched::job {
        class req_write {
        public:
            std::shared_ptr<object> obj;
            std::string property;
            std::shared_ptr<void> value;

            req_write(const std::shared_ptr<object>& obj_, std::string property_, const std::shared_ptr<void>& value)
                : obj(obj_), property(std::move(property_)), value(value) {}
        };

        // TODO: A more efficient way to handle this would be to execute game logic in an orderly manner
        ///      A lock-free queue may not even be required, however it is a precautionary measure. See
        ///      the mutex for more clarification on why this was done.
        tbb::concurrent_queue<std::unique_ptr<req_write>> _rw_queue;

    public:
        // The presence of this mutex is a precautionary measure. While reads and writes are currently
        // executed through a DAG, making the reads and writes atomic, having a mutex ensures safe access 
        // to object properties in scenarios where data may be accessed from a thread outside the scheduler,
        // preventing potential data races.
        std::mutex mutex{};

        void execute() override;

        void enqueue_write(const std::shared_ptr<object>& obj, const std::string& property, std::shared_ptr<void>& value);
    };
}
