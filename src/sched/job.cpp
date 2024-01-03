#include "job.hpp"

namespace sched {
    void job::schedule(const std::shared_ptr<job>& child) {
        if (child.get() == this)
            throw std::runtime_error("Cyclic job dependency detected");

        {
            std::lock_guard lock{ child->job_mutex };
            child->_parent = this;
        }

        std::lock_guard lock{ job_mutex };
        _children.push_back(child);
    }

    void job::erase(const std::shared_ptr<job>& child) {
        {
            std::lock_guard lock{ child->job_mutex };
            child->_parent = nullptr;
        }

        std::lock_guard lock{ job_mutex };
        std::erase(_children, child);
    }
}
