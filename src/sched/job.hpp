#pragma once

#include <memory>
#include <atomic>
#include <mutex>
#include <vector>

namespace sched {
    // Forward declarations
    class worker;

    // Represents a task that can be executed by a worker
    class job : public std::enable_shared_from_this<job> {
        // Pointer to the worker that this job is assigned to
        std::atomic<sched::worker*> _worker;

        // Pointer to the parent job
        // This is not atomic because this is more important to
        // be consistent
        job* _parent{};

        // A collection of nth-sub-cycle jobs that are scheduled to be executed
        std::vector<std::shared_ptr<job>> _children;

        // Exit code (reserved for future use)
        int _exit_code{};

        // Determines if the job is scheduled to be re-executed
        bool _exited{ false };

    public:
        // Mutex for the children collection
        std::recursive_mutex job_mutex;

        // Returns a shared pointer to this job.
        std::shared_ptr<job> ref() {
            return shared_from_this();
        }

        // Run the task
        virtual void execute() = 0;

        // Destroy the job and any resources it may have allocated
        virtual ~job() = default;

        // Get the worker that this job is assigned to
        [[nodiscard]] auto worker() const { return _worker.load(); }

        // Get a collection of child jobs
        [[nodiscard]] auto& children() const { return _children; }

        // Get the parent job
        [[nodiscard]] auto parent() const { return _parent; }

        // Stop scheduling the job
        void exit(const int exit_code = 0) {
            // These are OK to be set non-atomically since they are only
            // accessed by the arbiter thread during a non-execution phase
            _exit_code = exit_code;
            _exited = true;
        }

        // Add a child job
        void schedule(const std::shared_ptr<job>& child);

        // Remove a child job
        void erase(const std::shared_ptr<job>& child);

        // Friend classes
        friend class runner;
        friend class worker;
    };
}
