#include "ExternalTransferScreen.hpp"
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

    Component ExternalTransferScreen(
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

                RefreshAccounts();
                RefreshOtherUsers();

                from_menu = Dropdown(&account_labels, &selected_from_idx);
                
                recipient_mode_toggle = Toggle(&recipient_mode_list, &selected_mode_idx);
                
                // Recipient Selectors
                user_menu = Dropdown(&other_user_labels, &selected_user_idx);
                manual_input = Input(&manual_address, "Recipient Public Key");

                amount_input = Input(&amount_str, "Amount");

                btn_transfer = Button("Transfer Funds", [this] {
                    OnTransfer();
                });

                container = Container::Vertical({
                    from_menu,
                    recipient_mode_toggle,
                    Container::Tab({
                        user_menu,
                        manual_input
                    }, &selected_mode_idx),
                    amount_input,
                    btn_transfer
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

            void RefreshOtherUsers() {
                auto all_users = dbService_->getAllUsers();
                other_users_.clear();
                other_user_labels.clear();
                
                for (const auto& u : all_users) {
                    if (u.publicKey != user_.publicKey) {
                        other_users_.push_back(u);
                        other_user_labels.push_back(u.name + " (" + u.publicKey.substr(0, 4) + "...)");
                    }
                }
                
                if (other_user_labels.empty()) {
                    other_user_labels.push_back("No other users found");
                }
            }

            void OnTransfer() {
                if (accounts_.empty()) {
                    status_text = "Error: No source accounts.";
                    status_color = Color::Red;
                    return;
                }

                uint64_t amount = utils::SolanaUtils::ParseTokenAmount(amount_str);
                if (amount == 0) {
                    status_text = "Error: Invalid amount.";
                    status_color = Color::Red;
                    return;
                }

                std::string recipientPubKey;
                if (selected_mode_idx == 0) { // Local User
                    if (other_users_.empty()) {
                        status_text = "Error: No other users to select.";
                        status_color = Color::Red;
                        return;
                    }
                    recipientPubKey = other_users_[selected_user_idx].publicKey;
                } else { // Manual
                    if (manual_address.empty()) {
                        status_text = "Error: Recipient address required.";
                        status_color = Color::Red;
                        return;
                    }
                    recipientPubKey = manual_address;
                }

                status_text = "Processing transfer...";
                status_color = Color::Yellow;

                try {
                    std::string keyPath = utils::ConfigManager::getSessionKeypairPath();
                    auto keypair = utils::SolanaUtils::ReadKeypairFromKeypairFile(keyPath);
                    if (!keypair) {
                        status_text = "Error: Auth failed.";
                        status_color = Color::Red;
                        return;
                    }
                    std::string privKeyBase58 = utils::EncodeBase58(*keypair);

                    auto& from_acc = accounts_[selected_from_idx];

                    std::string result = relayService_->ExternalTransfer(
                        from_acc.seedIndex,
                        recipientPubKey,
                        amount,
                        user_.publicKey,
                        privKeyBase58
                    );

                    status_text = "Success! Sig: " + result;
                    status_color = Color::Green;
                    amount_str.clear();

                } catch (const std::exception& e) {
                    status_text = "API Error: " + std::string(e.what());
                    status_color = Color::Red;
                }
            }

            Element Render() override {
                // Check if accounts changed
                auto current_accounts = dbService_->getAccountsForUser(user_.publicKey);
                if (current_accounts.size() != accounts_.size()) {
                    RefreshAccounts();
                }
                
                // Ideally refresh other users too, but that's less frequent
                
                bool is_local_user = (selected_mode_idx == 0);

                return vbox({
                    text("External Transfer") | bold | center,
                    separator(),
                    text("From Account:"),
                    from_menu->Render() | border,
                    separator(),
                    text("Recipient Mode:"),
                    recipient_mode_toggle->Render(),
                    text(is_local_user ? "Select User:" : "Enter Address:"),
                    (is_local_user ? user_menu->Render() : manual_input->Render()) | border,
                    separator(),
                    text("Amount:"),
                    amount_input->Render() | border,
                    separator(),
                    btn_transfer->Render() | center,
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
            int selected_from_idx = 0;

            std::vector<models::User> other_users_;
            std::vector<std::string> other_user_labels;
            int selected_user_idx = 0;

            std::vector<std::string> recipient_mode_list = {"Local User", "Manual Address"};
            int selected_mode_idx = 0;

            std::string manual_address;
            std::string amount_str;
            
            Component from_menu;
            Component recipient_mode_toggle;
            Component user_menu;
            Component manual_input;
            Component amount_input;
            Component btn_transfer;
            Component container;

            std::string status_text;
            Color status_color = Color::Default;
        };

        return Make<Impl>(relayService, dbService, user);
    }
}
