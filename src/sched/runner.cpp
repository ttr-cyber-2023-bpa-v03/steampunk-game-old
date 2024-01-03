#include "runner.hpp"

#include <algorithm>
#include <bitset>
#include <random>
#include <ranges>

#include "worker.hpp"
#include "job.hpp"

namespace sched {
    // We can assume that our core count is static due to the nature of the
    // operating system. This is highly unlikely to change because it would
    // break the OS to change the amount of logical cores on the system
    std::uint32_t runner::_core_count = std::thread::hardware_concurrency();

    void runner::push_worker() {
        // Get the next worker ID
        const auto next_worker_id = _worker_id_counter++;

        // Instantiate the new worker and prepare it for execution
        // This will auto-increment the worker ID counter
        auto new_worker = std::make_unique<worker>(this, next_worker_id, next_affinity_mask());

        // Ownership hack to access the object after moving it
        const auto raw_worker = new_worker.get(); 

        // Acquire a lock on the worker pool and push the worker
        // This will transfer the thread ownership to the pool
        std::lock_guard lock{ _worker_pool_mutex };
        _worker_pool.emplace(raw_worker->id(), std::move(new_worker));

        // We can now start the worker thread
        // This is done while locked because we don't want to have a
        // non-running worker in the pool
        raw_worker->detach();
    }

    bool runner::pop_worker() {
        // Get the worker ID
        const auto worker_id = --_worker_id_counter;

        // Acquire a lock on the worker pool
        std::lock_guard lock{ _worker_pool_mutex };

        // If there are no workers in the pool, we can't pop a worker
        // because there are no workers to pop
        if (_worker_pool.empty())
            return false;

        // Transfer ownership and remove
        std::unique_ptr<worker> worker = std::move(_worker_pool[worker_id]);
        _worker_pool.erase(worker_id);

        // RAII will destroy the worker
        return true;
    }

    void runner::assign_job(
        const std::shared_ptr<job>& job,
        std::uint32_t& next_worker_id,
        std::vector<std::shared_ptr<sched::job>>& run_next
    ) noexcept {
        // We're looking for the least busy worker, so we need to assign them
        // from low id -> high id and wrap around. This ensures an even job
        // distribution between workers

        // If the job has exited, erase it from the scheduler
        if (job->_exited) {
            erase(job);
            return;
        }

        // If the worker ID is higher than the worker ID counter, we need to
        // reset the worker ID counter to 0 so we can start from the beginning
        if (next_worker_id == _worker_id_counter)
            next_worker_id = 0;

        // Acquire a lock on the worker pool and extract the worker
        std::lock_guard lock{ _worker_pool_mutex };

        // Find the worker
        const auto& worker_pair = _worker_pool.find(next_worker_id++);
        if (worker_pair == _worker_pool.end()) {
            // Check if there are any more workers
            if (_worker_pool.empty() || next_worker_id == 1)
                return;

            // Worker was likely popped; decrement and try again
            next_worker_id -= 2; // -2 because we incremented earlier
            assign_job(job, next_worker_id, run_next);
            return;
        }
        const auto& worker = worker_pair->second;

        // Assign the job to the worker
        worker->assign(job);
        worker->wake();

        // These jobs will run on the next sub-cycle
        std::lock_guard lock2{ job->job_mutex };
        for (const auto& child : job->children())
            run_next.push_back(child);
    }

    void runner::wait_for_workers() const noexcept {
        for (auto worker_id = 0u; worker_id < _worker_id_counter; worker_id++) {
            worker* worker;
            {
                std::lock_guard lock{ _worker_pool_mutex };
                const auto& worker_pair = _worker_pool.find(worker_id);
                if (worker_pair == _worker_pool.end())
                    break; // Handle synchronization edge case
                worker = worker_pair->second.get();
            }
            worker->wait_cycle_finish();
        }
    }

    void runner::runner_arbiter() {
        // Double-buffer for the job collections
        std::vector<std::shared_ptr<job>> jobs_1{};
        std::vector<std::shared_ptr<job>> jobs_2{};

        // Determines the next worker to use
        std::uint32_t next_worker_id{};

        // Each of these cycles are rendered as a "full cycle",
        // defining a full frame in the scheduler
        while (_active) {
            time_point cycle_start;

            // Acquire a lock on the root job collection and copy the jobs
            // to a temporary buffer that we can safely use
            {
                std::lock_guard lock{ _root_jobs_mutex };
                std::ranges::copy(_root_jobs, std::back_inserter(jobs_1));
            }

            // Save the start time of the cycle, we will use it later to
            // calculate the time delta between cycles
            cycle_start = clock::now();

            // Traverse all jobs and assign them to workers
            // This is the zeroth sub-cycle of the scheduler
            for (auto& job : jobs_1)
                 assign_job(job, next_worker_id, jobs_2);

            // Prepare the jobs buffer for the next sub-cycle by clearing it
            // This buffer will be used for (n>1)th sub-cycle jobs
            jobs_1.clear();

            // Wait for all workers to suspend (sub-cycle finished)
            wait_for_workers();

            // This is the nth sub-cycle loop of the scheduler that assigns
            // deferred jobs to workers
            while (!jobs_2.empty()) {
                for (auto& job : jobs_2)
                    assign_job(job, next_worker_id, jobs_1);
                
                // Prepare the buffers for the next sub-cycle and wait for
                // all workers to suspend (sub-cycle finished)
                jobs_2.clear();
                std::swap(jobs_1, jobs_2);
                wait_for_workers();
            }

            // Write the execution delta
            const auto cycle_end = clock::now();
            auto exec_delta = std::chrono::duration_cast<duration>(cycle_end - cycle_start).count();

            // Calculate the remaining time. If we beat the frame delay, we
            // are achieving the desired frame rate
            auto remaining_time = std::max(0.0, frame_delay - exec_delta);
            if (remaining_time > 0) {
                exec_delta += remaining_time;
                std::this_thread::sleep_for(duration(remaining_time));
            }

            // Write the cycle delta
            cycle_delta = exec_delta;

            // Clear the temporary buffer and do a swap
            jobs_2.clear();
            std::swap(jobs_1, jobs_2);
        }

        // Pop all workers from the pool
        while (pop_worker()) {}

        // Report finished
        std::lock_guard lock{ _arbiter_close_mutex };
        _arbiter_close_cv.notify_all();
    }

    runner::~runner() {
        // Stop the scheduler if it's running
        stop();
    }

    // Affinity generation could definitely be improved. For now, we just
    // pin one thread to each core. This is not ideal because the OS can
    // hog specific cores, but it's better than nothing.

    // A better algorithm would be ideal, but this is not a priority. The
    // scheduler actually runs pretty well with this affinity generation
    // algorithm.

    platform::affinity_mask runner::next_affinity_mask() {
        // If there are no unused cores, we can't push a worker because
        // we ran out of cores to use, and pushing more workers would
        // be a waste of resources
        if (_unused_cores.load() == 0)
            throw std::runtime_error("No more cores available");

        // Create a custom affinity mask bound to a the core located at
        // the LSB that is set in the current mask
        const auto worker_affinity = _unused_cores & ~(_unused_cores - 1);

        // Remove the core from the unused core mask
        _unused_cores &= ~worker_affinity;

        // Return the affinity mask
        return worker_affinity;
    }

    void runner::start(const bool floating, const std::optional<std::uint32_t> thread_count) {
        if (_active)
            throw std::runtime_error("Scheduler is already running");

        auto worker_count = thread_count.value_or(_core_count);
        if (floating) worker_count--; // Last core for the arbiter

        // Push initial workers to the pool
        for (auto i = 0u; i < worker_count; i++)
            push_worker();

        // Set the active flag to true
        _active = true;

        // Create the arbiter thread
        std::thread arbiter_thread{ &runner::runner_arbiter, this };

        // Begin execution of the arbiter
        if (floating) {
            platform::set_thread_affinity(arbiter_thread, next_affinity_mask());
            arbiter_thread.detach();
        }
        else {
            arbiter_thread.join();
        }
    }

    void runner::stop() {
        if (!_active)
            return;

        // Set the active flag to false
        _active = false;

        // Wait for the arbiter to stop
        std::unique_lock lock{ _arbiter_close_mutex };
        _arbiter_close_cv.wait(lock);
    }

    void runner::signal_stop() {
        if (!_active)
            return;
        _active = false;
    }

    void runner::schedule(const std::shared_ptr<job>& to_schedule) {
        // Acquire a lock on the root job collection and push the job
        std::lock_guard lock{ _root_jobs_mutex };
        _root_jobs.push_back(to_schedule);
    }

    void runner::erase(const std::shared_ptr<job>& to_erase) {
        // If the job has a parent then we have to do this differently
        if (to_erase->parent() != nullptr) {
            to_erase->parent()->erase(to_erase);
            return;
        }

        // Acquire a lock on the root job collection
        std::lock_guard lock{ _root_jobs_mutex };

        // Find the job in the collection
        // It's faster to do this without copying over since we're
        // going to traverse the collection here anyways. This prevents
        // our time complexity from being O(2n) and instead O(n)
        const auto it = std::ranges::find(_root_jobs, to_erase);

        // If the job is not in the collection, we can't erase it
        if (it == _root_jobs.end())
            throw std::runtime_error("Cannot find job");

        // Transfer ownership. The job will destruct per RAII
        auto extracted_job = std::move(*it);
        _root_jobs.erase(it);
    }

    std::size_t runner::job_count() {
        std::size_t size;
        std::vector<std::shared_ptr<job>> sub_jobs{};

        {
            std::lock_guard lock{ _root_jobs_mutex };
            size = _root_jobs.size();

            for (const auto& job : _root_jobs) {
                size += job->children().size();
                sub_jobs.push_back(job);
            }
        }

        while (!sub_jobs.empty()) {
            const auto current_job = sub_jobs.back().get();
            sub_jobs.pop_back();

            std::lock_guard lock{ current_job->job_mutex };
            for (const auto& job : current_job->children()) {
                size += job->children().size();
                sub_jobs.push_back(job);
            }
        }

        return size;
    }
}
