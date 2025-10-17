#include "database.h"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace geo {

    std::string Database::unquote_(std::string s) {
        if (!s.empty() && s.front() == '"' && s.back() == '"' && s.size() >= 2) {
            s = s.substr(1, s.size() - 2);
        }

        return s;
    }

    //example of string 
    //37253632,"37254143","US","United States of America","Ohio","Russia","40.234720","-84.410280"

    void Database::split_csv_line_(const std::string &line, std::vector<std::string> &out) {
        out.clear();
        
        std::string cur;
        bool q = false;
        
        for (char c : line) {
            if (c == '"') {
                q = !q;
                cur.push_back(c);
            } else if (c == ',' && !q) {
                out.push_back(cur);
                cur.clear();
            } else {
                cur.push_back(c);
            }
        }
        
        out.push_back(cur);
    }

    bool Database::parse_u32_(const std::string &s, uint32_t &out) {
        std::string t = unquote_(s);

        if (t.empty()) return false;

        try {
            unsigned long v = std::stoul(t);
            
            if (v > 0xFFFFFFFFul) return false;

            out = static_cast<uint32_t>(v);

            return true;
        } catch (...) {
            return false;
        }
    }

    bool Database::load_csv(const std::string &path, std::string &error) {
        error.clear();
        records_.clear();

        std::ifstream in(path);
        if (!in) {
            error = "cannot open file: " + path;
            return false;
        }

        std::string line;
        std::vector<std::string> cols;
        records_.reserve(200000);

        while (std::getline(in, line)) {
            if (line.empty()) continue;

            split_csv_line_(line, cols);

            if (cols.size() < 6) continue;

            uint32_t start = 0;
            uint32_t end = 0;

            if (!parse_u32_(cols[0], start)) continue;
            if (!parse_u32_(cols[1], end)) continue;
            if (end < start) continue;

            const std::string countryCode = unquote_(cols[2]);
            const std::string cityName = unquote_(cols[5]);

            std::string label = countryCode + "," + cityName;
            
            Record record;
            record.start = start;
            record.end = end;
            record.label = label;

            records_.push_back(record);
        }

        if (records_.empty()) {
            error = "no valid rows parsed";
            return false;
        }

        std::sort(records_.begin(), records_.end(), [](const Record &a, const Record &b) {
            return a.start < b.start;
        });

        merge_adjacent_();

        return true;
    }

    void Database::merge_adjacent_() {
        if (records_.empty()) return;

        std::vector<Record> merged;
        merged.reserve(records_.size());

        Record cur = records_[0];

        for (size_t i = 1; i < records_.size(); ++i) {
            const Record &r = records_[i];
            if (r.start == cur.end + 1 && r.label == cur.label) {
                cur.end = r.end;
            } else {
                merged.push_back(std::move(cur));
                cur = r;
            }
        }
        merged.push_back(std::move(cur));
        records_.swap(merged);
    }

    bool Database::lookup_str(const std::string &ip_str, std::string &out_label) const {
        uint32_t ip = 0;
        
        if (!parse_ip(ip_str, ip)) return false;

        return lookup(ip, out_label);
    }

    bool Database::lookup(uint32_t ip, std::string &out_label) const {
        if (records_.empty()) return false;

        size_t lo = 0, hi = records_.size();
        while (lo < hi) {
            size_t mid = (lo + hi) / 2;
            if (records_[mid].start <= ip) {
                lo = mid + 1;
            } else {
                hi = mid;
            }
        }

        if (lo == 0) return false;

        const Record &cand = records_[lo - 1];

        if (ip <= cand.end) {
            out_label = cand.label;
            
            return true;
        }

        return false;
    }
}

