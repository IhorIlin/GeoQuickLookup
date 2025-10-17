#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <optional>
#include "database.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "error: usage './sample_app <database.csv>'" << std::endl;
        return EXIT_FAILURE;
    }

    // Indicate that the app is ready
    std::cout << "READY" << std::endl;

    geo::Database db;
    bool isLoaded = false;

    for (std::string cmd; std::getline(std::cin, cmd);) {
        if (cmd.find("LOAD") == 0) {
            std::string error;

            if (db.load_csv(argv[1], error)) {
                isLoaded = true;

                std::cout << "OK" << std::endl;
            } else {
                std::cout << "ERR" << std::endl;
            }
      } else if (cmd.find("LOOKUP") == 0) {
            if (!isLoaded) {
                std::cerr << "error: Lookup requested before database was ever loaded" << std::endl;
                return EXIT_FAILURE;
            }
            
            std::string ip = cmd.substr(7);
            std::string label;

            if (db.lookup_str(ip, label)) {
                std::cout << label << std::endl;
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
