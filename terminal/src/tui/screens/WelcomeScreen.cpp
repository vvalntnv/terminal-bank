#include "WelcomeScreen.hpp"
#include "../../utils/ConfigManager.hpp"
#include "../../utils/SolanaUtils.hpp"
#include "../../utils/Base58.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <stdlib.h>

using namespace ftxui;

namespace tui {
    namespace screens {

        Component WelcomeScreen(std::shared_ptr<services::DatabaseService> dbService, std::function<void()> onLoginSuccess) {
            class Impl : public ComponentBase {
            public:
                Impl(std::shared_ptr<services::DatabaseService> db, std::function<void()> onLogin) 
                    : dbService_(db), onLogin_(onLogin) {
                    
                    // --- Login Form Components ---
                    login_users = dbService_->getAllUsers();
                    for(const auto& u : login_users) {
                        user_labels.push_back(u.name + " (" + u.publicKey.substr(0, 4) + "...)");
                    }
                    
                    if (user_labels.empty()) {
                         user_labels.push_back("No users found");
                    }

                    user_menu = Menu(&user_labels, &selected_user_idx);
                    
                    login_button = Button("Login", [this] {
                        if (login_users.empty()) {
                            status_text = "No users to login.";
                            return;
                        }
                        
                        auto& user = login_users[selected_user_idx];
                        
                        // verify keyfile
                        auto pubKey = utils::SolanaUtils::ReadPublicKeyFromKeypairFile(key_path_login);
                        if (!pubKey) {
                            status_text = "Invalid keypair file.";
                            return;
                        }
                        
                        std::string derivedPub = utils::EncodeBase58(*pubKey);
                        if (derivedPub != user.publicKey) {
                            status_text = "Keypair does not match selected user.";
                            return;
                        }

                        // Success
                        utils::ConfigManager::saveSession(key_path_login);
                        onLogin_();
                    });

                    login_input = Input(&key_path_login, "Path to your id.json");

                    login_container = Container::Vertical({
                        user_menu,
                        login_input,
                        login_button
                    });


                    // --- Register Form Components ---
                    reg_button = Button("Create Account", [this] {
                         if (reg_name.empty() || reg_key_path.empty()) {
                             status_text = "Please fill all fields.";
                             return;
                         }

                         std::string finalKeyPath;

                         if (is_generating_new) {
                             // Logic 1: Key Management
                             // When generating new, reg_key_path is just the filename (e.g. "alice_key")
                             // We construct the full path: ~/.tbank_keys/alice_key.json
                             std::string keysDir = utils::ConfigManager::getKeysDirectory();
                             
                             // Append .json if missing
                             std::string filename = reg_key_path;
                             if (filename.find(".json") == std::string::npos) {
                                 filename += ".json";
                             }
                             
                             finalKeyPath = keysDir + "/" + filename;
                             
                             // Generate new key
                             std::string cmd = "solana-keygen new --no-bip39-passphrase --outfile " + finalKeyPath + " > /dev/null 2>&1";
                             int ret = system(cmd.c_str());
                             if (ret != 0) {
                                 status_text = "Failed to generate keypair.";
                                 return;
                             }
                         } else {
                             // Import existing: path is absolute
                             finalKeyPath = reg_key_path;
                         }

                         auto pubKey = utils::SolanaUtils::ReadPublicKeyFromKeypairFile(finalKeyPath);
                         if (!pubKey) {
                             status_text = "Failed to read keypair.";
                             return;
                         }

                         models::User newUser;
                         newUser.name = reg_name;
                         newUser.age = std::stoi(reg_age.empty() ? "0" : reg_age);
                         newUser.phoneNumber = reg_phone;
                         newUser.publicKey = utils::EncodeBase58(*pubKey);
                         newUser.walletAddress = newUser.publicKey; // For now same

                         try {
                             dbService_->createUser(newUser);
                             utils::ConfigManager::saveSession(finalKeyPath);
                             
                             // Logic 2: Fix Screen Transition
                             // The callback passed from main.cpp posts a task to restart the loop
                             // We just need to ensure we call it.
                             onLogin_(); 
                             
                         } catch (const std::exception& e) {
                             status_text = "Error creating user: " + std::string(e.what());
                         }
                    });

                    // Toggles
                    key_source_toggle = Toggle(&toggle_list, &key_source_selected);

                    // Form
                    reg_input_name = Input(&reg_name, "Name");
                    reg_input_age = Input(&reg_age, "Age");
                    reg_input_phone = Input(&reg_phone, "Phone");
                    reg_input_key = Input(&reg_key_path, "Key Path / Filename");

                    reg_container = Container::Vertical({
                        reg_input_name,
                        reg_input_age,
                        reg_input_phone,
                        key_source_toggle,
                        reg_input_key,
                        reg_button
                    });

                    // Top Level Tab
                    tab_toggle = Toggle(&tab_values, &tab_selected);
                    
                    main_container = Container::Vertical({
                        tab_toggle,
                        Container::Tab({
                            login_container,
                            reg_container
                        }, &tab_selected)
                    });

                    Add(main_container);
                }

                Element Render() override {
                    is_generating_new = (key_source_selected == 1);

                    return vbox({
                        text("TBANK - Welcome") | bold | center | color(Color::Cyan),
                        separator(),
                        tab_toggle->Render() | align_right,
                        separator(),
                        (tab_selected == 0 ? RenderLogin() : RenderRegister()),
                        separator(),
                        text(status_text) | color(Color::Red) | center
                    }) | border | center;
                }

                Element RenderLogin() {
                    return vbox({
                        text("Select User:"),
                        user_menu->Render() | frame | size(HEIGHT, LESS_THAN, 5) | border,
                        separator(),
                        text("Enter Keypair Path:"),
                        login_input->Render() | border,
                        login_button->Render() | align_right
                    });
                }

                Element RenderRegister() {
                    std::string key_label = is_generating_new 
                        ? "Enter filename (saved to ~/.tbank_keys/):" 
                        : "Enter absolute path to keypair:";

                    return vbox({
                        text("Personal Details"),
                        reg_input_name->Render() | border,
                        hbox({
                            reg_input_age->Render() | border | flex,
                            reg_input_phone->Render() | border | flex
                        }),
                        separator(),
                        text("Wallet Setup"),
                        key_source_toggle->Render(),
                        text(key_label),
                        reg_input_key->Render() | border,
                        reg_button->Render() | align_right
                    });
                }

            private:
                std::shared_ptr<services::DatabaseService> dbService_;
                std::function<void()> onLogin_;
                
                // State
                std::string status_text;
                int tab_selected = 0;
                std::vector<std::string> tab_values = {"Login", "Register"};

                // Login
                std::vector<models::User> login_users;
                std::vector<std::string> user_labels;
                int selected_user_idx = 0;
                std::string key_path_login;
                Component user_menu;
                Component login_input;
                Component login_button;
                Component login_container;

                // Register
                std::string reg_name;
                std::string reg_age;
                std::string reg_phone;
                std::string reg_key_path;
                int key_source_selected = 0;
                bool is_generating_new = false;

                Component reg_input_name;
                Component reg_input_age;
                Component reg_input_phone;
                Component reg_input_key;
                Component key_source_toggle;
                Component reg_button;
                Component reg_container;

                Component tab_toggle;
                Component main_container;
                
                std::vector<std::string> toggle_list = {"Import Existing", "Generate New"};
            };

            return Make<Impl>(dbService, onLoginSuccess);
        }

    }
}
