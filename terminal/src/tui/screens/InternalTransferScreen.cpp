#include "InternalTransferScreen.hpp"
#include "../../utils/ConfigManager.hpp"
#include "../../utils/SolanaUtils.hpp"
#include "../../utils/Base58.hpp"
#include "../../models/Account.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <vector>

using namespace ftxui;

namespace tui::screens {

    Component InternalTransferScreen(
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

                from_menu = Dropdown(&account_labels, &selected_from_idx);
                to_menu = Dropdown(&account_labels, &selected_to_idx);
                
                amount_input = Input(&amount_str, "Amount");

                btn_transfer = Button("Transfer Funds", [this] {
                    OnTransfer();
                });

                container = Container::Vertical({
                    from_menu,
                    to_menu,
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

            void OnTransfer() {
                if (accounts_.size() < 2) {
                    status_text = "Error: Need at least 2 accounts for internal transfer.";
                    status_color = Color::Red;
                    return;
                }

                if (selected_from_idx == selected_to_idx) {
                    status_text = "Error: Cannot transfer to same account.";
                    status_color = Color::Red;
                    return;
                }

                uint64_t amount = utils::SolanaUtils::ParseTokenAmount(amount_str);
                if (amount == 0) {
                    status_text = "Error: Invalid amount.";
                    status_color = Color::Red;
                    return;
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
                    auto& to_acc = accounts_[selected_to_idx];

                    std::string result = relayService_->InternalTransfer(
                        from_acc.seedIndex,
                        to_acc.seedIndex,
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
                auto current_accounts = dbService_->getAccountsForUser(user_.publicKey);
                if (current_accounts.size() != accounts_.size()) {
                    RefreshAccounts();
                }

                return vbox({
                    text("Internal Transfer") | bold | center,
                    separator(),
                    text("From Account:"),
                    from_menu->Render() | border,
                    text("To Account:"),
                    to_menu->Render() | border,
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
            int selected_to_idx = 0;

            std::string amount_str;
            
            Component from_menu;
            Component to_menu;
            Component amount_input;
            Component btn_transfer;
            Component container;

            std::string status_text;
            Color status_color = Color::Default;
        };

        return Make<Impl>(relayService, dbService, user);
    }
}
