# steampunk-game

An over-engineered RTS steampunk game written in C++ and SDL2.

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