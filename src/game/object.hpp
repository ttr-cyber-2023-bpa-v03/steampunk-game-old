#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

// Declare a getter function
#define SG_DECL_GET(type, name) \
    [[nodiscard]] type get_##name() const;

// Implement a getter function
#define SG_IMPL_GET(type, name, ...) \
    [[nodiscard]] type get_##name() const __VA_ARGS__

// Implement a getter function that wraps a private field prefixed with an underscore
#define SG_IMPL_GET_WRAP(name) \
    [[nodiscard]] decltype(_##name) get_##name() const { \
        return _##name; \
    }

// Declare a setter function
#define SG_DECL_SET(type, name) \
    void set_##name(const type value);

// Implement a setter function
#define SG_IMPL_SET(type, name, ...) \
    void set_##name(const type value) __VA_ARGS__

// Implement a setter function that wraps a private field prefixed with an underscore
#define SG_IMPL_SET_WRAP(name) \
    void set_##name(const decltype(_##name) value) { \
        _##name = value; \
    }

// Aquire a lock on the write job and call the unsafe getter.
// Intended for use with threads outside of the scheduler.
#define SG_SYNGET(obj, name) \
    { \
        std::lock_guard lock(::game::world::instance()->write_job->mutex); \
        return obj->get_##name(); \
    }

// Aquire a lock on the write job and call the unsafe setter.
// Intended for use with threads outside of the scheduler.
#define SG_SYNSET(obj, name, ...) \
    { \
        std::lock_guard lock(::game::world::instance()->write_job->mutex); \
        obj->set_##name(__VA_ARGS__); \
    }

namespace game {
    // This represents the most abstract form of an object. It is intended to be
    // used as a base class for all objects in the game.
    class object : public std::enable_shared_from_this<object> {
        // The name of the object. This is a read-only property.
        std::string _name;

        // A list of children to the object. This is used to allow for the creation
        // of a hierarchy of objects.
        std::vector<std::shared_ptr<object>> _children{};

    public:
        object(const std::string_view name) : _name(name) {}

        // Returns a shared pointer to this object.
        std::shared_ptr<object> ref() {
            return shared_from_this();
        }

        [[nodiscard]] auto name() const { return _name; }

        // Retreive a list of children to the object. Note that this creates a copy, so
        // use sparingly.
        [[nodiscard]] auto children() const { return _children; }

        // Add an object as a child to this object.
        void add_child(const std::shared_ptr<object> &p_child);

        // Add an object as a child to this object.
        void add_child(object& child) {
            add_child(child.ref());
        }

        // Remove an object as a child to this object.
        void remove_child(const std::shared_ptr<object> &p_child);

        // Remove an object as a child to this object.
        void remove_child(object& child) {
            remove_child(child.ref());
        }

        // Find first child of name in the children.
        [[nodiscard]] object& find_child(const std::string_view name) const;
    };
}