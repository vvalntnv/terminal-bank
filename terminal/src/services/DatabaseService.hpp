#pragma once

#include "../infra/database/SQLiteWrapper.hpp"
#include "../models/User.hpp"
#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace services {

class DatabaseService {
public:
    DatabaseService(const std::string& dbPath);
    void init();

    // User Operations
    void createUser(const models::User& user);
    std::vector<models::User> getAllUsers();
    std::optional<models::User> getUserByPublicKey(const std::string& pubKey);

private:
    std::unique_ptr<infra::database::SQLiteWrapper> db_;
};

} // namespace services
