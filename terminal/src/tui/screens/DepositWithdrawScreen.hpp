#pragma once
#include <ftxui/component/component.hpp>
#include <memory>
#include "../../services/RelayService.hpp"
#include "../../services/DatabaseService.hpp"
#include "../../models/User.hpp"

namespace tui::screens {
    ftxui::Component DepositWithdrawScreen(
        std::shared_ptr<services::RelayService> relayService,
        std::shared_ptr<services::DatabaseService> dbService,
        const models::User& user
    );
}
