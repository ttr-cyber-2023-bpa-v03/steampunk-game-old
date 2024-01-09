#include "worker.hpp"

#include <iostream>

#include "runner.hpp"
#include "job.hpp"

namespace sched {
    void worker::worker_main() {
        // Create a lock at first, because creating locks could be
        // expensive and we don't want to do it every cycle, especially
        // since this section of code may run more than 2x per frame
        std::unique_lock lock(mutex, std::defer_lock);

        try {
            // Each of these cycles are rendered as a "sub-cycle"
            // They consist of one portion of a frame
            while (true) {
                // Suspend until we are woken up
                {
                    std::lock_guard guard(lock);
                    _wake_cv.wait(lock, [this] { return _awake.load() || !_active; });
                }

                // Activity check
                if (!_active)
                    break;

                // Save the start time of the cycle, we will use it later to
                // calculate the time delta between cycles
                const auto cycle_start = runner::clock::now();

                // Traverse all jobs and execute them
                std::shared_ptr<job> current_job;
                while (_jobs.try_pop(current_job)) {
                    try {
                        current_job->execute();
                    }
                    catch (const std::exception& ex) {
                        // TODO: Better error handling
                        std::cout << "job execution exception: " << ex.what() << std::endl;
                    }
                }

                // Write the cycle delta
                const auto cycle_end = runner::clock::now();
                cycle_delta = std::chrono::duration_cast<runner::duration>(cycle_end - cycle_start).count();

                // Set the awake flag to false and notify that we are done
                _awake = false;
                {
                    std::lock_guard guard(lock);
                    _wake_cv.notify_one();
                }
            }
        }
        catch (const std::exception& ex) {
            // TODO: Better error handling
            std::cout << "worker crashed lol: " << ex.what() << std::endl;
        }

        // Set the awake flag to false and notify that the thread is done
        _awake = false;
        {
            std::lock_guard guard(lock);
            _wake_cv.notify_one();
        }
    }

    worker::worker(sched::runner* runner, const std::uint32_t id, const platform::affinity_mask affinity) {
        // Initialize object
        _runner = runner;
        _id = id;
        _thread = std::thread{ &worker::worker_main, this };
        
        // Set the thread's affinity, allowing the OS to schedule it on specific cores
        // If affinity is zero, the OS will handle affinity automatically
        if (affinity != 0)
            platform::set_thread_affinity(_thread, affinity);

        // Save the affinity for later use (reserved for future use)
        _affinity = affinity;
    }

    worker::~worker() {
        // Join the thread to ensure that it can properly
        // destroy itself if not started
        if (_thread.joinable())
            _thread.join();

        // Stop the worker
        stop();
    }

    void worker::assign(const std::shared_ptr<job>& job) {
        // No lock is needed since these are both lock-free
        job->_worker = this;
        _jobs.push(job);
    }

    void worker::detach() {
        if (_active)
            throw std::runtime_error("worker is already detached");

        _active = true;
        _thread.detach();
    }

    void worker::stop() {
        if (!_active)
            throw std::runtime_error("worker is already stopped");

        // These two variables are atomic, so we don't need to lock
        _active = false;
        _awake = true;

        // Notify the worker of var change and wait for another cycle to finish
        {
            std::unique_lock lock(mutex);
            _wake_cv.notify_one();
            _wake_cv.wait(lock, [this] { return !_awake; });
        }
    }

    void worker::wake() {
        // Atomic, no need to lock
        _awake = true;

        // Notify the worker of var change
        {
            std::unique_lock lock(mutex);
            _wake_cv.notify_one();
        }
    }

    void worker::wait_cycle_finish() {
        std::unique_lock lock(mutex);
        _wake_cv.wait(lock, [this] { return !_awake; });
    }
}
