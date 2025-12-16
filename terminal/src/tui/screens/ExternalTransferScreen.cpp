#include "ExternalTransferScreen.hpp"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace tui::screens {
    Component ExternalTransferScreen() {
        return Renderer([] {
            return vbox({
                text("External Transfer") | bold | center,
                separator(),
                text("TODO: Form to transfer to other users") | dim | center
            });
        });
    }
}
