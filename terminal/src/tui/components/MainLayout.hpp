#pragma once
#include <ftxui/component/component.hpp>
#include <string>
#include <functional>
#include <memory>
#include "../../services/RelayService.hpp"
#include "../../services/DatabaseService.hpp"
#include "../../models/User.hpp"

namespace tui::components {
    ftxui::Component MainLayout(
        const std::string& userName, 
        std::function<void()> onLogout,
        std::shared_ptr<services::RelayService> relayService,
        std::shared_ptr<services::DatabaseService> dbService,
        const models::User& user
    );
}
