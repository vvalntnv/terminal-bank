#include "HomeScreen.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace tui {
    namespace screens {

        Component HomeScreen(const std::string& userName, std::function<void()> onLogout) {
            class Impl : public ComponentBase {
            public:
                Impl(const std::string& name, std::function<void()> logout) 
                    : userName_(name), onLogout_(logout) {
                    
                    logout_btn = Button("Logout", [this] {
                        onLogout_();
                    });

                    Add(Container::Vertical({
                        logout_btn
                    }));
                }

                Element Render() override {
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
                        text("Welcome, " + userName_) | bold | center,
                        filler(),
                        logout_btn->Render() | align_right,
                    }) | border;
                }

            private:
                std::string userName_;
                std::function<void()> onLogout_;
                Component logout_btn;
            };

            return Make<Impl>(userName, onLogout);
        }

    }
}
