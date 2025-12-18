#pragma once
#include <ftxui/component/component.hpp>
#include <functional>
#include <string>
#include <memory>
#include "../../services/DatabaseService.hpp"
#include "../../models/User.hpp"

namespace tui {
    namespace screens {

        ftxui::Component HomeScreen(
            const models::User& user, 
            std::shared_ptr<services::DatabaseService> dbService,
            std::function<void()> onLogout
        );

    }
}
