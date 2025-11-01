#pragma once
#include "geo/bin_format.h"
#include <string>

namespace geo {

class Database {
    public:
    Database() = default;
    ~Database();
    
    bool load(const std::string &path);
    void close();
    const char * lookup(uint32_t ip) const;
    bool loaded() const { return header_ != nullptr; }

    private:
    int fileDescriptor_ = -1;
    size_t fileSize_ = 0;
    void *base_ = nullptr;

    const geo::Header *header_ = nullptr;
    const geo::Entry *entries_ = nullptr;
    const char *stringTable_ = nullptr;
};

}
