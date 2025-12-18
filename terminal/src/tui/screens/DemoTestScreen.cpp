#include "DemoTestScreen.hpp"
#include "../../utils/SolanaUtils.hpp"
#include "../../utils/Base58.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>

using namespace ftxui;

namespace tui {
    namespace screens {

        Component DemoTestScreen() {
            class Impl : public ComponentBase {
            public:
                Impl() {
                    input_option.multiline = false;
                    
                    // Button to load the keypair
                    button = Button("Load Keypair", [this] {
                        auto pubKeyBytes = utils::SolanaUtils::ReadPublicKeyFromKeypairFile(filePath);
                        if (pubKeyBytes) {
                            std::string base58 = utils::EncodeBase58(*pubKeyBytes);
                            output_text = "Public Key (Base58): " + base58;
                        } else {
                            output_text = "Error: Invalid keypair file or path not found.";
                        }
                    });

                    // Input component
                    input = Input(&filePath, "Enter absolute path to id.json", input_option);
                    
                    // Layout
                    Add(Container::Vertical({
                        input,
                        button,
                    }));
                }

                Element Render() override {
                    return vbox({
                        text("Demo Test Screen: Load Solana Keypair") | bold | center,
                        separator(),
                        text("File Path:"),
                        input->Render() | border,
                        button->Render() | align_right,
                        separator(),
                        text(output_text) | color(Color::Green),
                    }) | border;
                }

            private:
                std::string filePath;
                std::string output_text = "Waiting for input...";
                Component input;
                Component button;
                InputOption input_option;
            };

            return Make<Impl>();
        }

    }
}
