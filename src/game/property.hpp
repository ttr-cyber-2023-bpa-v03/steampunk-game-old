#pragma once

#include "write_job.hpp"

#include <memory>
#include <string>

#include "state.hpp"

namespace game {
    class object;

    class property_base {
        std::string _name;

        virtual void unsafe_write(std::shared_ptr<void>& value) = 0;
    public:
        property_base(const std::string_view name): _name(name) {}

        virtual ~property_base() = default;

        [[nodiscard]] auto name() const { return _name; }

        [[nodiscard]] virtual std::shared_ptr<void> read() const = 0;

        virtual void write(std::shared_ptr<void>& value) = 0;

        friend class object;
    };

    template <class T>
    class property final : public property_base {
        std::shared_ptr<T> _value;

        // Non-thread-safe write. Intended to be called by write_job.
        // Note that the cast is also unsafe here. The caller has the
        // responsibility to ensure that the value is of the correct
        // type listed in this object.
        void unsafe_write(std::shared_ptr<void>& value) override {
            _value = static_cast<T*>(value.get());
        }

    public:
        property(const std::string_view name) : property_base(name) {}

        [[nodiscard]] std::shared_ptr<void> read() const override {
            return _value;
        }

        [[nodiscard]] std::shared_ptr<T> read() {
            return _value;
        }

        // This is the thread-safe version of get(). It is intended for
        // use by threads outside of the scheduler where R/W collisions
        // may occur. This function will block until the write job has
        // finished writing to all objects.
        [[nodiscard]] auto async_get() const {
            std::lock_guard lock(state::j_write->mutex);
            return _value;
        }

        // Friend classes
        friend class write_job;
    };
}
