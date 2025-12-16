#include "services/DatabaseService.hpp"
#include "services/RelayService.hpp"
#include "infra/relay/RelayAPIClient.hpp"
#include "utils/ConfigManager.hpp"
#include "utils/SolanaUtils.hpp"
#include "utils/Base58.hpp"
#include "tui/screens/WelcomeScreen.hpp"
#include "tui/components/MainLayout.hpp"
#include "tui/components/GlobalWrapper.hpp"
#include <ftxui/component/screen_interactive.hpp>
#include <iostream>
#include <memory>

int main() {
    auto screen = ftxui::ScreenInteractive::Fullscreen();
    auto dbService = std::make_shared<services::DatabaseService>("terminal.db");
    
    // Initialize Relay Infrastructure
    auto relayClient = std::make_shared<infra::RelayAPIClient>("http://localhost:3000");
    auto relayService = std::make_shared<services::RelayService>(relayClient);

    try {
        dbService->init();
    } catch (const std::exception &e) {
        std::cerr << "Error initializing database: " << e.what() << std::endl;
        return 1;
    }

    // Callbacks to switch screens by exiting the current loop
    auto onLogout = [&]() {
        utils::ConfigManager::clearSession();
        screen.ExitLoopClosure()(); 
    };

    auto onLoginSuccess = [&]() {
        screen.ExitLoopClosure()();
    };

    bool should_quit = false;
    auto onQuit = [&]() {
        should_quit = true;
        screen.ExitLoopClosure()();
    };

    // Main Application Loop
    while (true) {
        if (should_quit) break;

        // 1. Check for session
        std::string keypairPath = utils::ConfigManager::getSessionKeypairPath();
        ftxui::Component component;
        
        if (!keypairPath.empty()) {
            // Validate session
            auto pubKey = utils::SolanaUtils::ReadPublicKeyFromKeypairFile(keypairPath);
            if (pubKey) {
                std::string pubKeyStr = utils::EncodeBase58(*pubKey);
                auto user = dbService->getUserByPublicKey(pubKeyStr);
                
                if (user) {
                    // Valid Session -> Go Main Layout
                    component = tui::components::MainLayout(user->name, relayService, onLogout);
                }
            }
            
            if (!component) {
                // Invalid session found (file missing or user deleted) -> Clear it
                utils::ConfigManager::clearSession();
            }
        }

        // 2. No Session (or invalid) -> Show Welcome
        if (!component) {
             component = tui::screens::WelcomeScreen(dbService, relayService, onLoginSuccess);
        }
        
        // Wrap with Global Handler (Footer + Esc/Ctrl-C logic)
        auto wrapped_component = tui::components::GlobalWrapper(component, onQuit);

        // 3. Run the selected component
        screen.Loop(wrapped_component);
    }

    return 0;
}
