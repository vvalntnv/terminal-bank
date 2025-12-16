#include "InternalTransferScreen.hpp"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace tui::screens {
    Component InternalTransferScreen() {
        return Renderer([] {
            return vbox({
                text("Internal Transfer") | bold | center,
                separator(),
                text("TODO: Form to transfer between own accounts") | dim | center
            });
        });
    }
}
