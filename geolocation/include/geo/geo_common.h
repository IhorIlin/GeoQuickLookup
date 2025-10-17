#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <sstream>

namespace geo {

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
