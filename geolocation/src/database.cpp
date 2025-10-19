#include "database.h"
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace geo {

    Database::~Database() {
        close();
    }

    void Database::close() {
        if (base_) {
            munmap(base_, fileSize_);
            base_ = nullptr;
        }
        if (fileDescriptor_ >= 0) {
            ::close(fileDescriptor_);
            fileDescriptor_ = -1;
        }
        header_ = nullptr;
        entries_ = nullptr;
        stringTable_ = nullptr;
    }

    bool Database::load(const std::string &path) {
        close();

        fileDescriptor_ = open(path.c_str(), O_RDONLY);

        if (fileDescriptor_ < 0) {
            return false;
        }

        struct stat st;

        if (fstat(fileDescriptor_, &st) != 0) {
            close();
            return false;
        }

        fileSize_ = st.st_size;
        base_ = mmap(nullptr, fileSize_, PROT_READ, MAP_PRIVATE, fileDescriptor_, 0);
        if (base_ == MAP_FAILED) {
            base_ = nullptr;
            close();
            return false;
        }

        header_ = reinterpret_cast<const Header *>(base_);
        if (!header_valid(header_, fileSize_)) {
            close();
            return false;
        }

        entries_ = reinterpret_cast<const Entry *>(reinterpret_cast<const char *>(header_) + sizeof(Header));
        stringTable_ = reinterpret_cast<const char *>(entries_) + header_->count * sizeof(Entry);

         return true;
    }

    const char * Database::lookup(uint32_t ip) const {
       if (!header_) return nullptr;

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

