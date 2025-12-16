#pragma once
#include <ftxui/component/component.hpp>
#include <functional>

namespace tui {
    namespace components {

        ftxui::Component GlobalWrapper(ftxui::Component child, std::function<void()> onQuit);

    }
}
