#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "property.hpp"

namespace game {
    class object {
        std::string _name;

        std::vector<property_base> _properties;

        std::vector<std::shared_ptr<object>> _children;

        property_base* find_property(const std::string& property) {
            const auto it = std::ranges::find(_properties, property, &property_base::name);
            if (it == _properties.end())
                return nullptr;
            return &*it;
        }

        void set_unsafe(const std::string& property, std::shared_ptr<void> value) {
            auto prop = find_property(property);
            prop->unsafe_write(value);
        }

    public:
        object(const std::string_view name) : _name(name) {}

        [[nodiscard]] auto name() const { return _name; }

        [[nodiscard]] std::shared_ptr<void> get(const std::string& property) {
            auto prop = find_property(property);
            return prop->read();
        }

        void set(const std::string& property, std::shared_ptr<void> value) {
            auto prop = find_property(property);
            prop->write(value);
        }

        // Friend classes
        friend class write_job;
    };
}
