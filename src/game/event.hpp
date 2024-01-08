#pragma once

#include <mutex>
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <any>

namespace game {
    // Forward declaration
    class event_base;
    template <class T = std::any> class connection;
    template <class T = std::any> class event;

    template <class T>
    using event_callback = std::function<void(T)>;

    class connection_base : public std::enable_shared_from_this<connection_base> {
        // Determines if the callback will be invoked when the event is fired
        bool _alive = true;

        // The event that this connection is bound to
        std::shared_ptr<event_base> _event;

    public:
        // Callback wrapper
        struct cb_wrapper {
            std::function<void(std::any)> callback;

            cb_wrapper(std::function<void(std::any)> cb)
                : callback(cb) {}

            void operator()(std::any arg) {
                callback(arg);
            }
        };

    private:
        // The callback that will be invoked when the event is fired
        cb_wrapper _callback;

    public:
        connection_base(std::shared_ptr<event_base> event, std::function<void(std::any)> callback)
            : _event(event), _callback(callback) {}

        // Determine if this connection is attached to the event
        [[nodiscard]] bool alive() { return _alive; }

        // Get the event that this connection is bound to
        [[nodiscard]] std::shared_ptr<event_base> event() { return _event; }

        // Attach this connection to the event
        void attach() {
            _alive = true;
        }

        // Detach this connection from the event
        void detach();

        // Fire the callback
        void fire(std::any context);

        // Virtual destructor
        virtual ~connection_base() = default;

        // Friend classes
        friend class event_base;
        friend class connection<>;
    };

    class event_base : public std::enable_shared_from_this<event_base> {
    protected:
        // A collection of connections to this event
        std::vector<std::shared_ptr<connection_base>> _cn_vector;

    public:
        // Get the reference to this event
        [[nodiscard]] std::shared_ptr<event_base> ref() {
            return this->shared_from_this();
        }

        // Connect a callback to this event
        void connect(connection_base::cb_wrapper callback);

        // Connect a callback to this event
        void connect(std::function<void(std::any)> callback) {
            connect(connection_base::cb_wrapper(callback));
        }

        // Fire this event, invoking all connections
        void fire(std::any context);

        // Yield on this event, waiting for a connection to fire, then returning the result
        std::any yield();

        // Disconnect everything in this event
        void disconnect_all();

        // Virtual destructor
        virtual ~event_base() = default;

        // Friend classes
        friend class connection_base;
    };

    template <class T>
    class connection : public connection_base {
    public:
        using event_t = game::event<T>;
        using callback_t = event_callback<T>;

        connection(std::shared_ptr<event_t> event, callback_t callback)
            : connection_base(event, std::any(callback)) {}

        // Fire this connection, invoking the callback
        void fire(T context) {
            connection_base::fire(std::any(context));
        }

        // Friend classes
        template <class U> friend class event;
    };

    template <class T>
    class event : public event_base {
    public:
        using connection_t = connection<T>;

        // Connect a callback to this event
        void connect(event_callback<T> callback) {
            event_base::connect([callback](std::any arg) {
                callback(std::any_cast<T>(arg));
            });
        }

        // Fire this event, invoking all connections
        void fire(T context) {
            event_base::fire(std::any(context));
        }

        // Yield on this event, waiting for a connection to fire, then returning the result
        T yield() {
            return std::any_cast<T>(event_base::yield());
        }
    };
}