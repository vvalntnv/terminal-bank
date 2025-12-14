#include <iostream>
#include "services/DatabaseService.hpp"
#include "tui/screens/DemoTestScreen.hpp"
#include <ftxui/component/screen_interactive.hpp>

int main() {
    std::cout << "Starting Terminal Bank..." << std::endl;

    // Initialize Database (keep existing logic)
    try {
        services::DatabaseService dbService("terminal.db");
        dbService.init();
        std::cout << "Database initialized successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error initializing database: " << e.what() << std::endl;
        return 1;
    }

    // Start TUI
    auto screen = ftxui::ScreenInteractive::Fullscreen();
    auto component = tui::screens::DemoTestScreen();
    screen.Loop(component);

    return 0;
}
