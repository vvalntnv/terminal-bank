#include "DepositWithdrawScreen.hpp"
#include "../../utils/ConfigManager.hpp"
#include "../../utils/SolanaUtils.hpp"
#include "../../utils/Base58.hpp"
#include "../../models/Account.hpp"
#include "../../models/User.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <vector>

using namespace ftxui;

namespace tui::screens {

    Component DepositWithdrawScreen(
        std::shared_ptr<services::RelayService> relayService,
        std::shared_ptr<services::DatabaseService> dbService,
        const models::User& user
    ) {
        class Impl : public ComponentBase {
        public:
            Impl(std::shared_ptr<services::RelayService> relay,
                 std::shared_ptr<services::DatabaseService> db,
                 const models::User& u)
                : relayService_(relay), dbService_(db), user_(u) {

                // 1. Fetch Accounts
                RefreshAccounts();

                // 2. Components
                account_menu = Dropdown(&account_labels, &selected_account_idx);
                
                action_toggle = Toggle(&action_list, &selected_action_idx);
                
                amount_input = Input(&amount_str, "Amount");

                btn_execute = Button("Execute Transaction", [this] {
                    OnExecute();
                });

                // Layout
                container = Container::Vertical({
                    account_menu,
                    action_toggle,
                    amount_input,
                    btn_execute
                });

                Add(container);
            }

            void RefreshAccounts() {
                accounts_ = dbService_->getAccountsForUser(user_.publicKey);
                account_labels.clear();
                for (const auto& acc : accounts_) {
                    account_labels.push_back(acc.accountName + " (ID: " + std::to_string(acc.seedIndex) + ")");
                }
                if (account_labels.empty()) {
                    account_labels.push_back("No accounts found");
                }
            }

            void OnExecute() {
                if (accounts_.empty()) {
                    status_text = "Error: No accounts available.";
                    status_color = Color::Red;
                    return;
                }

                if (amount_str.empty()) {
                    status_text = "Error: Amount is required.";
                    status_color = Color::Red;
                    return;
                }

                uint64_t amount = 0;
                try {
                    amount = std::stoull(amount_str);
                } catch (...) {
                    status_text = "Error: Invalid amount format.";
                    status_color = Color::Red;
                    return;
                }

                if (amount == 0) {
                    status_text = "Error: Amount must be greater than 0.";
                    status_color = Color::Red;
                    return;
                }

                status_text = "Processing transaction...";
                status_color = Color::Yellow;

                try {
                    // 1. Get Keys
                    std::string keyPath = utils::ConfigManager::getSessionKeypairPath();
                    auto keypair = utils::SolanaUtils::ReadKeypairFromKeypairFile(keyPath);
                    if (!keypair) {
                        status_text = "Error: Could not read session keys.";
                        status_color = Color::Red;
                        return;
                    }
                    std::string privKeyBase58 = utils::EncodeBase58(*keypair);

                    // 2. Identify Account
                    auto& selected_acc = accounts_[selected_account_idx];

                    // 3. Execute
                    std::string result;
                    bool is_deposit = (selected_action_idx == 0);

                    if (is_deposit) {
                        result = relayService_->Deposit(
                            selected_acc.seedIndex,
                            amount,
                            user_.publicKey,
                            privKeyBase58
                        );
                    } else {
                        result = relayService_->Withdraw(
                            selected_acc.seedIndex,
                            amount,
                            user_.publicKey,
                            privKeyBase58
                        );
                    }

                    status_text = "Success! Sig: " + result.substr(0, 16) + "...";
                    status_color = Color::Green;
                    amount_str.clear();

                } catch (const std::exception& e) {
                    status_text = "API Error: " + std::string(e.what());
                    status_color = Color::Red;
                }
            }

            // Refetch accounts every render just in case (for local prototype speed)
            // In a real app we'd use an event or manual refresh.
            Element Render() override {
                // Check if accounts changed count (simple check)
                // This is a bit hacky but ensures if user creates account it shows up here
                // without needing full complex state management
                auto current_accounts = dbService_->getAccountsForUser(user_.publicKey);
                if (current_accounts.size() != accounts_.size()) {
                    RefreshAccounts();
                }

                return vbox({
                    text("Deposit & Withdraw") | bold | center,
                    separator(),
                    text("Select Account:"),
                    account_menu->Render() | border,
                    separator(),
                    text("Action:"),
                    action_toggle->Render() | center,
                    separator(),
                    text("Amount:"),
                    amount_input->Render() | border,
                    separator(),
                    btn_execute->Render() | center,
                    separator(),
                    text(status_text) | color(status_color) | center
                }) | border;
            }

        private:
            std::shared_ptr<services::RelayService> relayService_;
            std::shared_ptr<services::DatabaseService> dbService_;
            models::User user_;

            std::vector<models::Account> accounts_;
            std::vector<std::string> account_labels;
            int selected_account_idx = 0;

            std::vector<std::string> action_list = {"Deposit", "Withdraw"};
            int selected_action_idx = 0;

            std::string amount_str;
            
            Component account_menu;
            Component action_toggle;
            Component amount_input;
            Component btn_execute;
            Component container;

            std::string status_text;
            Color status_color = Color::Default;
        };

        return Make<Impl>(relayService, dbService, user);
    }
}
