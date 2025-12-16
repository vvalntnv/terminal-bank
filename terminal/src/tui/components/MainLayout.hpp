#pragma once
#include <ftxui/component/component.hpp>
#include <string>
#include <functional>
#include <memory>
#include "../../services/RelayService.hpp"

namespace tui::components {
    ftxui::Component MainLayout(
        const std::string& userName, 
        std::shared_ptr<services::RelayService> relayService,
        std::function<void()> onLogout
    );
}
