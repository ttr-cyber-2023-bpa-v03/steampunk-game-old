# steampunk-game

An over-engineered RTS steampunk game written in C++ and SDL2.

## Building

This project uses CMake to build. It is intended to be built using Linux, but it can also
be built using other platforms. To use the build script, ensure that you have CMake,
Ninja, and SDL2 installed. Then, run the following commands:

```bash
./build.sh
```

This will generate a Ninja setup using CMake, and then build the project. The executable
will be located in the `build` directory, along with any other build files. This script
will also automatically copy the assets to the `build` directory.

In other platforms, or independent of the build script, the project can be built using
CMake and your preferred build system. Here's an example of how to build the project
using CMake and Make:

```bash
mkdir build
cd build
cmake ..
make
```

If you are using Windows, you can use Visual Studio to build the project. To do so,
install Visual Studio and CMake, and then run the following commands:

```bash
mkdir build
cd build
cmake ..
```

This will generate a Visual Studio solution file, which can be opened in Visual Studio.
From there, you can select your platform and build the project.

TODO: Auto copy assets on Windows w/ VS.

## Running

To run the project, simply run the executable in the `build` directory. The executable
will be named `steampunk-game`. The executable can be run from any directory, as long
as the `assets` directory is in the same directory as the executable.

## Engine

The primary focus of this project is the engine, which is designed to be a robust,
flexible, and adaptable framework for creating games in C++. The engine is designed
with the following goals in mind:

- **Efficiency**: The top goal of this engine is to be efficient and optimized,
    with the goal of being able to achieve high frame rates if desired.

- **Objects**: The engine is designed to be object-oriented, meaning that all
    objects in the game are represented by instances of classes. This allows for
    controlled and managed access to all objects in the game.

- **Multithreading**: The engine is designed to be multithreaded, allowing for
    multiple tasks to be handled at once. This is done using a DAG-based task
    scheduler, allowing certain tasks to be run in parallel, while others are
    run sequentially.

- **Events**: The engine is designed to be event-driven, meaning that all
    user input and other events are handled by the engine and passed to
    the appropriate objects.

- **Cross-platform**: The engine is designed to be cross-platform, meaning that
    it can be compiled and run on both Windows and Linux systems. At the time,
    support for Mac OS is not supported.

- **Error Handling**: The engine is designed to be robust and handle errors
    gracefully, meaning that extensive error handling is implemented to ensure
    that the engine does not crash or behave unexpectedly.

- **Debugging**: The engine is designed to be debuggable, meaning that it
    provides debugging tools, such as logging, to help debug issues.