#include "event.hpp"
#include "event_pump.hpp"
#include "world.hpp"

#include <future>

namespace game {
    void event_base::connect(connection_base::cb_wrapper callback) {
        auto cn = std::make_shared<connection_base>(ref(), callback); // err
        _cn_vector.push_back(cn);
    }

    void event_base::disconnect_all() {
        for (auto& cn : _cn_vector)
            cn->detach();
        _cn_vector.clear();
    }

    void event_base::fire(std::any context) {
        for (auto& cn : _cn_vector) {
            if (cn->alive())
                cn->fire(context);
        }
    }

    std::any event_base::yield() {
        // Create a promise and a future
        std::promise<std::any> promise;
        auto future = promise.get_future();
    
        // Connect a callback to this event
        connect([&promise](std::any context) {
            // Set the promise value
            promise.set_value(context);
        });

        // Wait for the promise to be set
        future.wait();
        return future.get();
    }

    void connection_base::detach() {
        _alive = false;
        std::ranges::remove(_event->_cn_vector, shared_from_this());
    }

    void connection_base::fire(std::any context) { 
        event_invocation_request req{ _callback, context };
        world::instance()->event_pump->enqueue(req);
    }
}