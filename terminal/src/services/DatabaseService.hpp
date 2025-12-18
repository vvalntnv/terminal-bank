#pragma once

#include "../infra/database/SQLiteWrapper.hpp"
#include "../models/User.hpp"
#include "../models/Account.hpp"
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

    // Account Operations
    void createAccount(const models::Account& account);
    int getNextSeedIndex();
    std::vector<models::Account> getAccountsForUser(const std::string& userPubKey);
    void updateAccountBalance(int seedIndex, const std::string& balance);

private:
    std::unique_ptr<infra::database::SQLiteWrapper> db_;
};

} // namespace services
