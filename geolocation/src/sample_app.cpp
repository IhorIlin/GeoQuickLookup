#include <iostream>
#include "database.h"
#include "geo/geo_common.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "error: usage './Sample_app <database.bin>'" << std::endl;
        return EXIT_FAILURE;
    }

    // Indicate that the app is ready
    std::cout << "READY" << std::endl;

    geo::Database db;
    const std::string db_path = argv[1];

    for (std::string cmd; std::getline(std::cin, cmd);) {
        if (cmd.find("LOAD") == 0) {
            if (db.load(db_path)) {
                std::cout << "OK" << std::endl;
            } else {
                std::cout << "ERR" << std::endl;
            }
        } else if (cmd.find("LOOKUP") == 0) {
            if (!db.loaded()) {
                std::cerr << "error: Lookup requested before database was ever loaded" << std::endl;
                return EXIT_FAILURE;
            }

            std::string ip_str = cmd.substr(7);
            uint32_t ip = 0;

            if (!geo::parse_ip(ip_str, ip)) {
                std::cout << "ERR" << std::endl;
                continue;
            }

            const char *result = db.lookup(ip);

            if (result) {
                std::cout << result << std::endl;
            } else {
                std::cout << "ERR" << std::endl;
            }
        } else if (cmd.find("EXIT") == 0) {
            std::cout << "OK" << std::endl;
            break;
        } else {
            std::cerr << "error: Unknown command received" << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
