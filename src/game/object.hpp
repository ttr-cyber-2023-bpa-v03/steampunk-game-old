#pragma once

#include "util/debug.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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