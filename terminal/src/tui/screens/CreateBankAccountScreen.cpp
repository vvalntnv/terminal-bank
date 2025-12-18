#include "CreateBankAccountScreen.hpp"
#include "../../utils/ConfigManager.hpp"
#include "../../utils/SolanaUtils.hpp"
#include "../../utils/Base58.hpp"
#include "../../models/Account.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>

using namespace ftxui;

namespace tui::screens {

    Component CreateBankAccountScreen(
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

                // Components
                input_name = Input(&account_name, "e.g. Vacation Fund");
                
                btn_create = Button("Create Account", [this] {
                    OnCreate();
                });

                // Layout
                container = Container::Vertical({
                    input_name,
                    btn_create
                });

                Add(container);
            }

            // Function to handle creation
            void OnCreate() {
                if (account_name.empty()) {
                    status_text = "Error: Account name cannot be empty.";
                    status_color = Color::Red;
                    return;
                }

                status_text = "Creating account...";
                status_color = Color::Yellow;

                try {
                    // 1. Get Session Keypair
                    std::string keyPath = utils::ConfigManager::getSessionKeypairPath();
                    auto keypair = utils::SolanaUtils::ReadKeypairFromKeypairFile(keyPath);
                    
                    if (!keypair) {
                        status_text = "Error: Could not read session keypair.";
                        status_color = Color::Red;
                        return;
                    }

                    // 2. Encode Private Key for API
                    // Note: In a real production app, we would handle sensitive memory better.
                    std::string privKeyBase58 = utils::EncodeBase58(*keypair);

                    // 3. Get Next ID
                    int next_id = dbService_->getNextSeedIndex();

                    // 4. API Call
                    // This creates the PDA on-chain. Returns likely a signature or PDA address.
                    std::string pda_address = relayService_->CreateAccount(
                        next_id, 
                        account_name, 
                        user_.publicKey, 
                        privKeyBase58
                    );

                    // 5. Local Persistence
                    models::Account newAccount;
                    newAccount.seedIndex = next_id;
                    newAccount.accountName = account_name;
                    newAccount.pubKey = user_.publicKey;
                    newAccount.pdaPubKey = pda_address; 
                    // Note: ATA is derived separately usually, but for now we might leave it empty or relay returns it?
                    // The project spec says relay returns PDA. 
                    
                    dbService_->createAccount(newAccount);

                    status_text = "Success! Account '" + account_name + "' created (ID: " + std::to_string(next_id) + ")";
                    status_color = Color::Green;
                    account_name.clear(); // Reset form

                } catch (const std::exception& e) {
                    status_text = "API Error: " + std::string(e.what());
                    status_color = Color::Red;
                }
            }

            Element Render() override {
                return vbox({
                    text("Create New Bank Account") | bold | center,
                    separator(),
                    hbox({
                        text("Account Name: "),
                        input_name->Render() | border | flex
                    }),
                    separator(),
                    btn_create->Render() | center,
                    separator(),
                    text(status_text) | color(status_color) | center
                }) | border;
            }

        private:
            std::shared_ptr<services::RelayService> relayService_;
            std::shared_ptr<services::DatabaseService> dbService_;
            models::User user_;

            std::string account_name;
            std::string status_text;
            Color status_color = Color::Default;

            Component input_name;
            Component btn_create;
            Component container;
        };

        return Make<Impl>(relayService, dbService, user);
    }
}
