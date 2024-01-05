#pragma once

#if defined(_WIN32)

#include <thread>

#define WIN32_LEAN_AND_MEAN
#    include <Windows.h>
#undef WIN32_LEAN_AND_MEAN

// Breaks std::min and std::max. Thanks Microsoft for making horrible macros!
#undef min
#undef max

namespace platform {
	using affinity_mask = DWORD_PTR;

	void set_thread_affinity(std::thread& thread, affinity_mask mask);

	void close_on_term();
}

#else

#warning "Included Windows system abstraction layer on a non-windows platform. This is probably a mistake."

#endif