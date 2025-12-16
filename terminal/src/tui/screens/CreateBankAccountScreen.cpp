#include "CreateBankAccountScreen.hpp"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace tui::screens {
    Component CreateBankAccountScreen() {
        return Renderer([] {
            return vbox({
                text("Create Bank Account") | bold | center,
                separator(),
                text("TODO: Form to create new PDA account") | dim | center
            });
        });
    }
}
