#pragma once

#include "sched/job.hpp"
#include <any>
#include <memory>
#include <queue>
#include <unordered_map>
#include "event.hpp"

namespace game {
    struct event_invocation_request {
        std::function<void(std::any)> callback;
        std::any context;
    };

    class event_pump : public sched::job {
        std::queue<event_invocation_request> requests;

    public:
        void execute() override {
            while (!requests.empty()) {
                auto req = requests.front();
                req.callback(req.context);
                requests.pop();
            }
        }

        void enqueue(event_invocation_request req) {
            requests.push(req);
        }
    };
}