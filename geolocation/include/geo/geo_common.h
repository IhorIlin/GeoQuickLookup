#pragma once
#include <string>
#include <sstream>

namespace geo {

    inline std::string unquote(std::string s) {
        if (s.size() >=2 && s.front() == '"' && s.back() == '"') {
            return s.substr(1, s.size() - 2);
        }

        return s;
    }

    inline bool parse_ip(const std::string &s, uint32_t &out) {
        unsigned a, b, c, d;
        char dot;

        std::istringstream ss(s);

        if ((ss >> a >> dot >> b >> dot >> c >> dot >> d) && dot == '.' &&
            a < 256 && b < 256 && c < 256 && d < 256) {
            out = (a << 24) | (b << 16) | (c << 8) | d;

            return true;
        }

        return false;
    }

    struct Record {
        uint32_t start;
        uint32_t end;
        std::string label;
    };

}
