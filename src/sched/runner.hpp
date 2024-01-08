#pragma once

#include <condition_variable>
#include <memory>
#include <atomic>
#include <mutex>
#include <chrono>
#include <unordered_map>

#include <tbb/concurrent_hash_map.h>

#include "platform/current.hpp"
#include "platform/linux.hpp"

namespace sched {
    // Forward declarations
    class worker;
    class job;

    // Runner class used to schedule jobs
    // This class is responsible for the creation of workers and the delegation of jobs to workers
    class runner {
        // The amount of logical cores available on the system
        static std::uint32_t _core_count;

        // A bitmask that represents the cores that are available for use
        // Initially set the mask to have all bits set
        std::atomic<platform::affinity_mask> _unused_cores = (1 << _core_count) - 1;

        // A number that represents the next worker ID
        std::atomic<std::uint32_t> _worker_id_counter;

        // A collection of workers that are currently running
        std::unordered_map<std::uint32_t, std::unique_ptr<worker>> _worker_pool;

        // Ensures mutual exclusion of the worker pool
    	mutable std::mutex _worker_pool_mutex;

        // A collection of zeroth-sub-cycle jobs that are scheduled to be executed
        // TODO: We can potentially use a concurrent data structure here as well,
        //       however, it is not necessary as the arbiter is quite fast as is.
        //       This idea is, however, worth revisiting in the future.
        std::vector<std::shared_ptr<job>> _root_jobs;

        // Ensures mutual exclusion of the root job collection
    	std::mutex _root_jobs_mutex;

        // Determines if the scheduler is running
        std::atomic<bool> _active{ false };

        // Condition variable used for reporting when the arbiter exits
		std::condition_variable _arbiter_close_cv{};

		// Mutex used for waiting on the arbiter to exit
		std::mutex _arbiter_close_mutex{};

        // Get the next thread affinity mask
        platform::affinity_mask next_affinity_mask();

        // Internal function used to assign a job to a worker
        void assign_job(const std::shared_ptr<job>& job, std::uint32_t& next_worker_id,
                        std::vector<std::shared_ptr<sched::job>>& run_next) noexcept;

        // Wait until all workers suspend
        void wait_for_workers() const noexcept;

        // The main loop of the scheduler
        void runner_arbiter();

    public:
        // Push a worker onto the pool
        void push_worker();

        // Pop a worker from the pool
        // Returns true if a worker existed and was popped
        bool pop_worker();

        // High resolution clock is used to determine more accurate time deltas between cycles
		using clock = std::chrono::high_resolution_clock;

		// These are just aliases for the types; saves space
		using time_point = clock::time_point;
		using duration = std::chrono::duration<double>;

        ~runner();

        // The amount of time a frame should last. Zero means that the scheduler should
        // attempt to match the refresh rate of the monitor.
        std::atomic<double> frame_delay{};

        // The time delta between two cycles
        std::atomic<double> cycle_delta{};

        // The amount of logical cores available on the system
        static auto core_count() { return _core_count; }

        // Start the task scheduler
        // The floating parameter determines if the scheduler should be detached
        void start(bool floating, std::optional<std::uint32_t> thread_count = std::nullopt);

        // Stop the task scheduler and wait for all jobs to finish
        // This is not safe to be called from a worker thread
        void stop();

        // Signal the task scheduler arbiter to stop scheduling jobs
        // This is OK to be called from a worker thread
        void signal_stop();

        // Schedule a job to be executed
        void schedule(const std::shared_ptr<job>& to_schedule);

        // Erase a job from the scheduler
        void erase(const std::shared_ptr<job>& to_erase);

        std::size_t job_count();

        friend class worker;
    };
}