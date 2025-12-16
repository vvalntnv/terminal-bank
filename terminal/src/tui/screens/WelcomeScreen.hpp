#pragma once
#include <ftxui/component/component.hpp>
#include <memory>
#include "../../services/DatabaseService.hpp"
#include "../../services/RelayService.hpp"

namespace tui {
    namespace screens {

        ftxui::Component WelcomeScreen(
            std::shared_ptr<services::DatabaseService> dbService,
            std::shared_ptr<services::RelayService> relayService,
            std::function<void()> onLoginSuccess
        );

    }
}
