#include "object.hpp"
#include "world.hpp"
#include "write_job.hpp"

namespace game {
    void object::add_child(const std::shared_ptr<object> &p_child) {
        // Queue child addition
        auto& writer = world::instance()->write_job;
        writer->enqueue([this, p_child] { _children.push_back(p_child); });
    }

    void object::remove_child(const std::shared_ptr<object> &p_child) {
        // Get the shared pointer to the child and find it
        const auto found = std::ranges::find(_children, p_child);

        // It's better to do find processing here than in the write job because if not
        // it would attribute to the write job's execution time, where no writing is
        // actually being done. In other words, it would add unnecessary overhead to
        // the write job. Instead, we process this in our own job (this function
        // should be called from some job's worker thread, but not the write job).

        // Queue child removal
        auto& writer = world::instance()->write_job;
        writer->enqueue([this, found] { _children.erase(found); });
    }

    object &object::find_child(const std::string_view name) const {
        const auto found = std::ranges::find_if(
            _children, [name](const auto &child) { return child->name() == name; });

        if (found == _children.end())
            throw std::runtime_error("Failed to find child under name " + std::string(name));

        return **found;
    }
}

