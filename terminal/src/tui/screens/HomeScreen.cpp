#include "HomeScreen.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <vector>
#include "../../models/Account.hpp"
#include "../../models/User.hpp"

using namespace ftxui;

namespace tui {
    namespace screens {

        Component HomeScreen(
            const models::User& user, 
            std::shared_ptr<services::DatabaseService> dbService,
            std::function<void()> onLogout
        ) {
            class Impl : public ComponentBase {
            public:
                Impl(const models::User& u, 
                     std::shared_ptr<services::DatabaseService> db,
                     std::function<void()> logout) 
                    : user_(u), dbService_(db), onLogout_(logout) {
                    
                    // Fetch accounts
                    accounts_ = dbService_->getAccountsForUser(user_.publicKey);

                    logout_btn = Button("Logout", [this] {
                        onLogout_();
                    });

                    Add(Container::Vertical({
                        logout_btn
                    }));
                }

                Element Render() override {
                    // Re-fetch or just use cached? For simplicity, using cached.
                    // If we want real-time updates, we might need a refresh mechanism or fetch in render (expensive).
                    // Ideally, we'd have a 'Refresh' button or event.
                    // For this prototype, fetching in constructor is okay, but if user adds account, they need to switch tabs to refresh.
                    // Let's refetch in Render for dynamic updates since this is a local DB read (fast enough).
                    accounts_ = dbService_->getAccountsForUser(user_.publicKey);

                    Elements rows;
                    // Header
                    rows.push_back(
                        hbox({
                            text("ID") | size(WIDTH, EQUAL, 5) | bold,
                            text("Name") | size(WIDTH, EQUAL, 20) | bold,
                            text("Public Key") | flex | bold
                        })
                    );
                    rows.push_back(separator());

                    if (accounts_.empty()) {
                        rows.push_back(text("No accounts found.") | dim);
                    } else {
                        for (const auto& acc : accounts_) {
                            rows.push_back(
                                hbox({
                                    text(std::to_string(acc.seedIndex)) | size(WIDTH, EQUAL, 5),
                                    text(acc.accountName) | size(WIDTH, EQUAL, 20),
                                    text(acc.pdaPubKey) | flex
                                })
                            );
                        }
                    }

                    return vbox({
                        text(R"(
  _______ ____   _    _   _ _  __
 |__   __|  _ \ / \  | \ | | |/ /
    | |  | |_) / _ \ |  \| | ' / 
    | |  |  __/ ___ \| . ` |  <  
    | |  | | / /   \ \ |\  | . \ 
    |_|  |_|/_/     \_\_| \_|_|\_\
                        )") | bold | center | color(Color::Cyan),
                        separator(),
                        text("Welcome, " + user_.name) | bold | center,
                        separator(),
                        text("My Accounts") | bold,
                        vbox(std::move(rows)) | border,
                        filler(),
                        logout_btn->Render() | align_right,
                    }) | border;
                }

            private:
                models::User user_;
                std::shared_ptr<services::DatabaseService> dbService_;
                std::function<void()> onLogout_;
                std::vector<models::Account> accounts_;
                Component logout_btn;
            };

            return Make<Impl>(user, dbService, onLogout);
        }

    }
}
