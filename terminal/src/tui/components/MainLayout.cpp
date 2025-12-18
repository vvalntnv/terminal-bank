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

    Component MainLayout(
        const std::string& userName, 
        std::function<void()> onLogout,
        std::shared_ptr<services::RelayService> relayService,
        std::shared_ptr<services::DatabaseService> dbService,
        const models::User& user
    ) {
        class Impl : public ComponentBase {
        public:
            Impl(
                const std::string& name, 
                std::function<void()> logout,
                std::shared_ptr<services::RelayService> relay,
                std::shared_ptr<services::DatabaseService> db,
                const models::User& u
            ) 
                : userName_(name), onLogout_(logout), relayService_(relay), dbService_(db), user_(u) {
                
                // 1. Initialize Screens
                auto home = screens::HomeScreen(u, db, logout);
                
                // Pass dependencies to CreateBankAccountScreen
                auto create_account = screens::CreateBankAccountScreen(relay, db, u);
                
                auto deposit_withdraw = screens::DepositWithdrawScreen(relay, db, u);
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

                Add(tab_container);
            }

            bool OnEvent(Event event) override {
                if (show_menu) {
                    if (event == Event::Escape || event == Event::F10) {
                        show_menu = false;
                        return true;
                    }
                    if (event == Event::Return) {
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

                if (event == Event::F10) {
                    show_menu = true;
                    return true;
                }

                return tab_container->OnEvent(event);
            }

            Element Render() override {
                Element content = tab_container->Render();

                if (show_menu) {
                    Element menu_visual = menu->Render() | border | center;
                    return dbox({
                        content | dim,
                        menu_visual | clear_under | center 
                    });
                }

                return content;
            }

        private:
            std::string userName_;
            std::function<void()> onLogout_;
            std::shared_ptr<services::RelayService> relayService_;
            std::shared_ptr<services::DatabaseService> dbService_;
            models::User user_;
            
            int active_tab_index = 0;
            Component tab_container;

            bool show_menu = false;
            int menu_selected_index = 0;
            std::vector<std::string> menu_entries;
            Component menu;
        };

        return Make<Impl>(userName, onLogout, relayService, dbService, user);
    }
}
