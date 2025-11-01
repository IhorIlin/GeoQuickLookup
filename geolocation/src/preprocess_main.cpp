#include <iostream>
#include <string>

bool preprocess_csv_to_bin(const std::string &csv_path, const std::string &bin_path);

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.csv> <output.bin>" << std::endl;
        return 1;
    }

    std::string csv_path = argv[1];
    std::string bin_path = argv[2];

    if (!preprocess_csv_to_bin(csv_path, bin_path)) {
        return 1;
    }

    std::cout << "Bin file created: " << bin_path << std::endl;

    return 0;
}