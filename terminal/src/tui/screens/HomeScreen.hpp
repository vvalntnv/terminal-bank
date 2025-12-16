#pragma once
#include <ftxui/component/component.hpp>
#include <functional>
#include <string>

namespace tui {
    namespace screens {

        ftxui::Component HomeScreen(const std::string& userName, std::function<void()> onLogout);

    }
}
