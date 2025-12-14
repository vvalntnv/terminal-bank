#include <iostream>
#include "services/DatabaseService.hpp"

int main() {
    std::cout << "Starting Terminal Bank..." << std::endl;

    try {
        services::DatabaseService dbService("terminal.db");
        dbService.init();
        std::cout << "Database initialized successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
