#include "database.h"
#include <algorithm>
#include <fstream>
#include <iostream>

namespace geo {

    bool Database::load(const std::string &path) {
        std::ifstream file(path, std::ios::binary | std::ios::ate);

        if (!file.is_open()) {
            std::cerr << "Failed to open file " << path << std::endl;
            return false;
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        buffer_.resize(size);

        if (!file.read(reinterpret_cast<char *>(buffer_.data()), size)) {
            std::cerr << "Failed to read file " << path << std::endl;
            return false;
        }

        file.close();

        header_ = reinterpret_cast<const geo::Header *>(buffer_.data());

        if (header_valid(header_, buffer_.size())) {
            std::cerr << "Failed to validate file " << std::endl;
            return false;
        }

        entries_ = reinterpret_cast<const Entry *>(buffer_.data() + sizeof(Header));

        stringTable_ = reinterpret_cast<const char *>(reinterpret_cast<const uint8_t *>(entries_) + header_->count * sizeof(Entry));

        return true;
    }

    const char * Database::lookup(uint32_t ip) const {
       if (!header_ || !entries_) return nullptr;

        size_t left = 0;
        size_t right = header_->count;

        while (left < right) {
            size_t middle = (left + right) / 2;
            const Entry &entry = entries_[middle];

            if (ip < entry.start) {
                right = middle;
            } else if (ip > entry.end) {
                left = middle + 1;
            } else {
                return stringTable_ + entry.label_offset;
            }
        }

        return nullptr;
    }
}

