#include "uri_tools.hpp"

#include "platform/current.hpp"

#include <iomanip>
#include <sstream>

namespace util {
    std::string url_encode(const std::string_view str) {
        std::ostringstream escaped{};

        // Set up formatting
        escaped.fill('0');
        escaped << std::hex;

        // Loop through each character
        for (auto c : str) {
            // Keep alphanumeric and other accepted characters intact
            if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
                continue;
            }

            // Any other characters are percent-encoded
            escaped << '%' << std::setw(2) << int((unsigned char) c);
        }

        return escaped.str();
    }


    std::string mailto::to_string() const {
        std::stringstream ss{};

        // Add the email address (meh for now just take my email)
        // Crude? Yes. Effective? Kinda. I'll fix later
        ss << "mailto:" << url_encode(email) << "?";
        ss << "subject=" << url_encode(subject) << "&";
        ss << "body=" << url_encode(body);

        return ss.str();
    }

    void mailto::open() const {
        // Open the mailto link
        platform::open_url(to_string());
    }
}