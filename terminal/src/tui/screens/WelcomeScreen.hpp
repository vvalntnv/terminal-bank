#pragma once
#include <ftxui/component/component.hpp>
#include <memory>
#include "../../services/DatabaseService.hpp"

namespace tui {
    namespace screens {

        ftxui::Component WelcomeScreen(std::shared_ptr<services::DatabaseService> dbService, std::function<void()> onLoginSuccess);

    }
}
