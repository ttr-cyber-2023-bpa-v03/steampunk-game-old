#pragma once

#include <condition_variable>
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>

#include <tbb/concurrent_queue.h>

#include "platform/current.hpp"

namespace sched {
	// Forward declarations
	class runner;
	class job;

	// Worker class used to execute jobs on separate threads
	// For internal task scheduling only. For more information, see the runner class.
	class worker {
		// Held here for ownership only. Not intended for usage.
		std::thread _thread;

		// The affinity mask assigned to the worker
		// Not thread-safe but only set once so it doesn't matter
		platform::affinity_mask _affinity;
		
		// The ID assigned to the worker
		// Not thread-safe but only set once so it doesn't matter
		std::uint32_t _id;

		sched::runner* _runner;

		// Determines if the thread can cycle
		std::atomic<bool> _active{ false };

		// Determines if the worker is awake
		std::atomic<bool> _awake = false;

		// Condition variable used for waking
		std::condition_variable _wake_cv{};

		// A collection of jobs that are scheduled to be executed
		tbb::concurrent_queue<std::shared_ptr<job>> _jobs{};

		// The entry point to a worker thread
		void worker_main();

	public:
		// Mutex for locking non-atomic operations
		std::mutex mutex{};

		// The time delta between two sub-cycles
		std::atomic<double> cycle_delta{ 0.0 };

		// Instantiate a new worker
		worker(sched::runner* runner, std::uint32_t id, platform::affinity_mask affinity = 0);

		// Destroy the worker
		~worker();

		// Get the worker's ID
		auto id() const { return _id; }

		// Get the affinity mask
		auto affinity() const { return _affinity; }

		// Get the runner associated with the worker
		auto runner() const { return _runner; }

		// Assign a job to the worker and resume the thread
		void assign(const std::shared_ptr<job>& job);

		// Runs the worker independently
		void detach();

		// Stop the worker
		void stop();

		// Force a cycle
		void wake();

		// Wait until the cycle is finished
		void wait_cycle_finish();
	};
}
