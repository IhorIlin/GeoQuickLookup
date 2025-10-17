#pragma once 
#include "geo/geo_common.h"
#include <string>
#include <vector>

namespace geo {

class Database {
    public:
    
    bool load_csv(const std::string &path, std::string &error);
    bool lookup_str(const std::string &ip_str, std::string &out_label) const;
    bool lookup(uint32_t ip, std::string &out_label) const;
    bool loaded() const { return !records_.empty(); }

    private:

    std::vector<Record> records_;
    void merge_adjacent_();
    static void split_csv_line_(const std::string &line, std::vector<std::string> &out);
    static std::string unquote_(std::string s);
    static bool parse_u32_(const std::string &s, uint32_t &uot);
};

}
