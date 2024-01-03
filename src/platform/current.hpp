#pragma once

#if defined(_WIN32)
#   include "windows.hpp"
#elif defined(__linux__)
#   include "linux.hpp"
#elif defined(__APPLE__)
    // Who even uses macOS for this kind of stuff?
    // Hey, feel free to implement this yourself and make a PR.
#   error "macOS is not supported because I don't have a Mac to test on"
#endif