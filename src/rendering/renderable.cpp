#include "renderable.hpp"
#include "render_job.hpp"

namespace rendering {
    std::atomic<std::uint32_t> renderable::_next_id = 0;
}