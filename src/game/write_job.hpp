#pragma once

#include <functional>
#include <tbb/concurrent_queue.h>

#include "sched/job.hpp"

// I settled with a little messier solution than the one i had in mind, but it works.
// Initially, I was working on a reflection system to allow for property management,
// but it would take a lot of blood, sweat, and tears to get it working. I decided
// to settle with a simpler solution that would allow for the same functionality and
// some additional flexibility.

// Eight hours down the drain, but at least I archived it and can come back to it
// later if I ever want to.

namespace game {
    class object;

    // This class is used to queue writes to objects. It is used to ensure that writes
    // are done in a synchonized manner, keeping other jobs like the renderer from
    // colliding with object information during a write and causing all sorts of nasty
    // bugs.
    class write_job final : public sched::job {
        // A list of functions to be invoked by the write job. These functions are to
        // write to the object.
        // By the way i wrapped this in a smart pointer because std::function doesn't
        // work well with the concurrent queue.
        tbb::concurrent_queue<std::unique_ptr<std::function<void()>>> queue;

    public:
        // The presence of this mutex is a precautionary measure. While reads and writes are currently
        // executed through a DAG, making the reads and writes atomic, having a mutex ensures safe access 
        // to object properties in scenarios where data may be accessed from a thread outside the scheduler,
        // preventing potential data races.
        std::mutex mutex{};

        // Ran by the scheduler to execute the queued functions.
        void execute() override;

        // Enqueue a function to be called during the write cycle.
        void enqueue(std::function<void()>&& fn);
    };
}
