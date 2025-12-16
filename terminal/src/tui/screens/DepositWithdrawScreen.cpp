#include "DepositWithdrawScreen.hpp"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace tui::screens {
    Component DepositWithdrawScreen() {
        return Renderer([] {
            return vbox({
                text("Deposit & Withdraw") | bold | center,
                separator(),
                text("TODO: List accounts and buttons") | dim | center
            });
        });
    }
}
