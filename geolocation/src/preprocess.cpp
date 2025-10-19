#include "geo/bin_format.h"
#include "geo/geo_common.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

struct Row {
    uint32_t start;
    uint32_t end;
    std::string countryCode;
    std::string cityName;
};

bool preprocess_csv_to_bin(const std::string &csv_path, const std::string &bin_path) {
    std::ifstream in(csv_path);

    if (!in) {
        std::cerr << "Failed to open file " << csv_path << std::endl;
        return false;
    }

    std::vector<Row> rows;
    rows.reserve(5000000);

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string start_str, end_str, countryCode, countryName, region, city;

        if (!std::getline(iss, start_str, ',')) continue;
        if (!std::getline(iss, end_str, ',')) continue;
        if (!std::getline(iss, countryCode, ',')) continue;
        std::getline(iss, countryName, ',');
        std::getline(iss, region, ',');
        std::getline(iss, city, ',');

        uint32_t start = std::stoul(geo::unquote(start_str));
        uint32_t end = std::stoul(geo::unquote(end_str));

        Row row = {start, end, countryCode, city};

        rows.push_back(row);
    }

    std::cout << "Loaded " << rows.size() << " rows" << std::endl;

    std::sort(rows.begin(), rows.end(), [](const Row &a, const Row &b) {
        return a.start < b.start;
    });

    std::ofstream out(bin_path, std::ios::binary);

    if (!out) {
        std::cerr << "Error: cannot create " << bin_path << std::endl;
        return false;
    }

    geo::Header header;
    std::memcpy(header.magic, "NORD", 4);
    header.count = static_cast<uint32_t>(rows.size());
    header.str_bytes = 0;

    out.write(reinterpret_cast<char *>(&header), sizeof(header));

    for (const auto &row : rows) {
        geo::Entry entry;
        entry.start = row.start;
        entry.end = row.end;

        std::string label = row.countryCode + "," + row.cityName;
        uint32_t offset = static_cast<uint32_t>(out.tellp());

        entry.label_offset = offset;

        out.write(reinterpret_cast<const char *>(&entry), sizeof(entry));
        out.write(label.c_str(), label.size() + 1);
    }

    std::cout << "Wrote binary file: " << bin_path << std::endl;

    return true;
}