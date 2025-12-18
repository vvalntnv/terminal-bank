#include "HomeScreen.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <vector>
#include <thread>
#include <atomic>
#include <iostream> // For debug
#include "../../models/Account.hpp"
#include "../../models/User.hpp"
#include "../../utils/SolanaUtils.hpp"
#include "../../utils/ConfigManager.hpp"
#include "../../utils/Base58.hpp"

using namespace ftxui;

namespace tui {
    namespace screens {

        Component HomeScreen(
            const models::User& user, 
            std::shared_ptr<services::DatabaseService> dbService,
            std::function<void()> onLogout,
            std::function<void(std::function<void()>)> post_task,
            std::shared_ptr<services::RelayService> relayService
        ) {
            class Impl : public ComponentBase {
            public:
                Impl(const models::User& u, 
                     std::shared_ptr<services::DatabaseService> db,
                     std::function<void()> logout,
                     std::function<void(std::function<void()>)> post_task,
                     std::shared_ptr<services::RelayService> relay)
                    : user_(u), dbService_(db), onLogout_(logout), post_task_(post_task), relayService_(relay) {
                    
                    // Fetch accounts initially from DB
                    accounts_ = dbService_->getAccountsForUser(user_.publicKey);

                    logout_btn = Button("Logout", [this] {
                        onLogout_();
                    });

                    Add(Container::Vertical({
                        logout_btn
                    }));
                }

                void RefreshBalances() {
                    if (is_loading_) return;
                    is_loading_ = true;

                    // Spawn thread for network calls
                    std::thread([this] {
                        // 1. Prepare keys (must be done or passed safely)
                        // Reading files in thread is okay.
                        std::string keyPath = utils::ConfigManager::getSessionKeypairPath();
                        auto keypair = utils::SolanaUtils::ReadKeypairFromKeypairFile(keyPath);
                        
                        if (!keypair) {
                            // Can't refresh without auth
                            is_loading_ = false;
                            return; 
                        }
                        std::string privKey = utils::EncodeBase58(*keypair);
                        std::string pubKey = user_.publicKey;

                        // 2. Iterate over local accounts
                        auto current_accounts = dbService_->getAccountsForUser(user_.publicKey);
                        
                        for (const auto& acc : current_accounts) {
                            // 3. Fetch from API (Authenticated)
                            std::string balanceStr = relayService_->GetBalance(acc.seedIndex, pubKey, privKey);
                            
                            // 4. Update DB
                            // Note: dbService is thread safe due to SQLite internal locking usually, 
                            // but ideally we should be careful. 
                            // SQLiteWrapper executes queries synchronously.
                            dbService_->updateAccountBalance(acc.seedIndex, balanceStr);
                        }

                        // 5. Post back to UI
                        post_task_([this] {
                            // Reload from DB to get fresh values
                            this->accounts_ = dbService_->getAccountsForUser(user_.publicKey);
                            this->is_loading_ = false;
                        });
                    }).detach();
                }

                bool OnEvent(Event event) override {
                    if (event == Event::Character('r')) {
                        RefreshBalances();
                        return true;
                    }
                    return ComponentBase::OnEvent(event);
                }

                Element Render() override {
                    // Refetch from DB on every render to catch updates from other screens (like CreateAccount)
                    // But ONLY if not currently refreshing to avoid jitter, or maybe always?
                    // Ideally we rely on the post_task to trigger the update.
                    // However, if user comes back from "Create Account", we want to see the new account.
                    // Simple approach: Always fetch from DB if not loading.
                    if (!is_loading_) {
                         accounts_ = dbService_->getAccountsForUser(user_.publicKey);
                    }

                    Elements rows;
                    // Header
                    rows.push_back(
                        hbox({
                            text("ID") | size(WIDTH, EQUAL, 5) | bold,
                            text("Name") | size(WIDTH, EQUAL, 20) | bold,
                            text("Balance") | size(WIDTH, EQUAL, 15) | bold, // New Column
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
                                    text(acc.balance + " LEV") | size(WIDTH, EQUAL, 15),
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
                        hbox({
                            text("My Accounts") | bold,
                            filler(),
                            is_loading_ ? text("Refreshing...") | color(Color::Yellow) : text("Press 'r' to refresh") | dim
                        }),
                        vbox(std::move(rows)) | border,
                        filler(),
                        logout_btn->Render() | align_right,
                    }) | border;
                }

            private:
                models::User user_;
                std::shared_ptr<services::DatabaseService> dbService_;
                std::function<void()> onLogout_;
                std::function<void(std::function<void()>)> post_task_;
                std::shared_ptr<services::RelayService> relayService_;
                
                std::vector<models::Account> accounts_;
                Component logout_btn;
                std::atomic<bool> is_loading_{false};
            };

            return Make<Impl>(user, dbService, onLogout, post_task, relayService);
        }

    }
}
