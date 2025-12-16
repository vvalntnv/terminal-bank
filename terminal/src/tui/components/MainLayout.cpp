#include "MainLayout.hpp"
#include "../screens/HomeScreen.hpp"
#include "../screens/CreateBankAccountScreen.hpp"
#include "../screens/DepositWithdrawScreen.hpp"
#include "../screens/InternalTransferScreen.hpp"
#include "../screens/ExternalTransferScreen.hpp"

#include <ftxui/dom/elements.hpp>
#include <iostream>

using namespace ftxui;

namespace tui::components {

    Component MainLayout(const std::string& userName, 
                         std::shared_ptr<services::RelayService> relayService, 
                         std::function<void()> onLogout) {
        class Impl : public ComponentBase {
        public:
            Impl(const std::string& name, 
                 std::shared_ptr<services::RelayService> relay, 
                 std::function<void()> logout) 
                : userName_(name), relayService_(relay), onLogout_(logout) {
                
                // 1. Initialize Screens
                // Note: HomeScreen already has a logout button, but we might want to handle logout centrally too.
                // For now, we pass the callback to HomeScreen as before.
                auto home = screens::HomeScreen(name, logout);
                auto create_account = screens::CreateBankAccountScreen();
                auto deposit_withdraw = screens::DepositWithdrawScreen();
                auto internal_transfer = screens::InternalTransferScreen();
                auto external_transfer = screens::ExternalTransferScreen();

                // 2. Tab Container
                tab_container = Container::Tab({
                    home,
                    create_account,
                    deposit_withdraw,
                    internal_transfer,
                    external_transfer
                }, &active_tab_index);

                // 3. Navigation Menu
                menu_entries = {
                    "Dashboard",
                    "Create Bank Account",
                    "Deposit / Withdraw",
                    "Internal Transfer",
                    "External Transfer",
                    "Logout"
                };
                
                menu = Menu(&menu_entries, &menu_selected_index);

                // 4. Modal Logic
                // The Modal overlay will contain the menu.
                // We use a custom render to show it over the content.
                
                // Initialize component tree
                Add(tab_container);
                // We don't Add(menu) directly because we handle it manually in OnEvent/Render
                // actually, for correct event handling of the modal, we should wrap logic.
            }

            // Custom Event Handling for 'n' and Menu interaction
            bool OnEvent(Event event) override {
                if (show_menu) {
                    // If menu is open, it captures events first
                    if (event == Event::Escape || event == Event::Character('n')) {
                        show_menu = false;
                        return true;
                    }
                    if (event == Event::Return) {
                        // Handle selection
                        if (menu_selected_index == 5) { // Logout
                            onLogout_();
                        } else {
                            active_tab_index = menu_selected_index;
                            show_menu = false;
                        }
                        return true;
                    }
                    return menu->OnEvent(event);
                }

                // If menu is closed, check for 'n' to open it
                if (event == Event::Character('n')) {
                    show_menu = true;
                    return true;
                }

                return tab_container->OnEvent(event);
            }

            Element Render() override {
                Element content = tab_container->Render();

                if (show_menu) {
                    // Create a modal overlay
                    Element menu_visual = menu->Render() | border | center;
                    
                    return dbox({
                        content | dim, // Dim the background
                        menu_visual | clear_under | center 
                    });
                }

                return content;
            }

        private:
            std::string userName_;
            std::shared_ptr<services::RelayService> relayService_;
            std::function<void()> onLogout_;
            
            int active_tab_index = 0;
            Component tab_container;

            // Navigation Menu State
            bool show_menu = false;
            int menu_selected_index = 0;
            std::vector<std::string> menu_entries;
            Component menu;
        };

        return Make<Impl>(userName, relayService, onLogout);
    }
}
