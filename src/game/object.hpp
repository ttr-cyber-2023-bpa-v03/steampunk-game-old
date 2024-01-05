#pragma once

#include "game/state.hpp"
#include "game/write_job.hpp"
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

        // For shared_from_this
        std::shared_ptr<object> getptr() {
            return shared_from_this();
        }

        [[nodiscard]] auto name() const { return _name; }

        // Retreive a list of children to the object. Note that this creates a copy, so
        // use sparingly.
        [[nodiscard]] auto children() const { return _children; }

        // Add an object as a child to this object.
        void add_child(object& child) {
            // Get the shared pointer to the child
            SG_LOCATE;
            const auto p_child = child.shared_from_this();
            
            // Queue child addition
            state::j_write->enqueue([this, p_child] {
                SG_LOCATE;
                _children.push_back(p_child);
            });
        }

        // Remove an object as a child to this object.
        void remove_child(object& child) {
            // Get the shared pointer to the child and find it
            const auto p_child = child.shared_from_this();
            const auto found = std::ranges::find(_children, p_child);

            // It's better to do find processing here than in the write job because if not
            // it would attribute to the write job's execution time, where no writing is
            // actually being done. In other words, it would add unnecessary overhead to
            // the write job. Instead, we process this in our own job (this function
            // should be called from some job's worker thread, but not the write job).

            // Queue child removal
            state::j_write->enqueue([this, found] {
                _children.erase(found);
            });
        }

        // Find first child of name in the children.
        [[nodiscard]] object& find_child(const std::string_view name) const {
            const auto found = std::ranges::find_if(_children, [name](const auto& child) {
                return child->name() == name;
            });

            if (found == _children.end())
                throw std::runtime_error("Failed to find child under name " + std::string(name));
            
            return **found;
        }
    };
}