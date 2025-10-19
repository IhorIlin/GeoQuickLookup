#pragma once
#include "geo/bin_format.h"
#include <string>
#include <vector>


namespace geo {

class Database {
    public:
    
    bool load(const std::string &path);
    const char * lookup(uint32_t ip) const;
    bool loaded() const { return header_ != nullptr; }

    private:

    const geo::Header *header_ = nullptr;
    const geo::Entry *entries_ = nullptr;
    const char *stringTable_ = nullptr;
    std::vector<uint8_t> buffer_;
};

}
