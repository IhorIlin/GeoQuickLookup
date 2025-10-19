#pragma once
#include <cstdint>
#include <cstring>

namespace geo {

    struct Header {
        char magic[4];
        uint32_t count;
        uint32_t str_bytes;
    };

    struct Entry {
        uint32_t start;
        uint32_t end;
        uint32_t label_offset;
    };

    inline bool header_valid(const Header *header, std::size_t file_size) {
        if (!header) return false;

        if (std::memcmp(header->magic, "NORD", 4) != 0) return false;

        if (sizeof(Header) + header->count * sizeof(Entry) + header->str_bytes >= file_size) return false;

        return true;
    }
}