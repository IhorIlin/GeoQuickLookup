#include "geo/bin_format.h"
#include "geo/geo_common.h"
#include <fstream>
#include <iostream>
#include <vector>

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

        Row row = {start, end, geo::unquote(countryCode), geo::unquote(city)};

        rows.push_back(row);
    }

    std::cout << "Loaded " << rows.size() << " rows" << std::endl;

    std::sort(rows.begin(), rows.end(), [](const Row &a, const Row &b) {
        return a.start < b.start;
    });

    std::vector<geo::Entry> entries;
    std::vector<char> string_table;
    entries.reserve(rows.size());

    for (const auto &row : rows) {
        std::string label = row.countryCode + "," + row.cityName;
        uint32_t offset = static_cast<uint32_t>(string_table.size());
        string_table.insert(string_table.end(), label.begin(), label.end());
        string_table.push_back('\0');

        geo::Entry entry;
        entry.start = row.start;
        entry.end = row.end;
        entry.label_offset = offset;

        entries.push_back(entry);
    }

    geo::Header header;
    std::memcpy(header.magic, "NORD", 4);
    header.count = static_cast<uint32_t>(entries.size());
    header.str_bytes = static_cast<uint32_t>(string_table.size());

    std::ofstream out(bin_path, std::ios::binary);

    if (!out) {
        std::cerr << "Error: cannot create " << bin_path << std::endl;
        return false;
    }

    out.write(reinterpret_cast<char *>(&header), sizeof(header));
    out.write(reinterpret_cast<char *>(entries.data()), entries.size() * sizeof(geo::Entry));
    out.write(string_table.data(), string_table.size());

    std::cout << "Wrote binary file: " << bin_path << std::endl;

    return true;
}