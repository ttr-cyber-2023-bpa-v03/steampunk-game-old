#pragma once

#include <string>
#include <unordered_map>

namespace util {
    std::string url_encode(const std::string_view str);

    class mailto {
        std::string email;

        std::string subject;

        std::string body;

    public:
        mailto() {};

        mailto(
            const std::string& email,
            const std::string& subject,
            const std::string& body
        ) : email(email), subject(subject), body(body) {};

        std::string to_string() const;
    };
}