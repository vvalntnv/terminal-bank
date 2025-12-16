#include "DatabaseService.hpp"
#include <iostream>
#include <sstream>

namespace services {

DatabaseService::DatabaseService(const std::string &dbPath) {
  db_ = std::make_unique<infra::database::SQLiteWrapper>(dbPath);
}

void DatabaseService::init() {
  // 1. User Table
  std::string createUserTable = "CREATE TABLE IF NOT EXISTS user ("
                                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                "name TEXT NOT NULL,"
                                "age INTEGER,"
                                "phone_number TEXT,"
                                "wallet_address TEXT,"
                                "public_key TEXT NOT NULL UNIQUE"
                                ");";
  db_->execute(createUserTable);

  // 2. Bank Accounts Table
  std::string createAccountTable = "CREATE TABLE IF NOT EXISTS account ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "seed_index INTEGER NOT NULL,"
                                   "account_name TEXT,"
                                   "pub_key TEXT,"
                                   "token_symbol TEXT DEFAULT 'LEV',"
                                   "pda_pub_key TEXT,"
                                   "ata_pub_key TEXT"
                                   ");";
  db_->execute(createAccountTable);

  // 3. Events Table
  std::string createEventsTable = "CREATE TABLE IF NOT EXISTS events ("
                                  "event_id TEXT PRIMARY KEY,"
                                  "type TEXT NOT NULL,"
                                  "pda_index_from TEXT,"
                                  "pda_index_to TEXT,"
                                  "amount INTEGER,"
                                  "timestamp INTEGER"
                                  ");";
  db_->execute(createEventsTable);
}

void DatabaseService::createUser(const models::User& user) {
    std::string sql = "INSERT INTO user (name, age, phone_number, wallet_address, public_key) VALUES ('" +
                      user.name + "', " +
                      std::to_string(user.age) + ", '" +
                      user.phoneNumber + "', '" +
                      user.walletAddress + "', '" +
                      user.publicKey + "');";
    db_->execute(sql);
}

std::vector<models::User> DatabaseService::getAllUsers() {
    std::string sql = "SELECT * FROM user;";
    auto results = db_->query(sql);
    
    std::vector<models::User> users;
    for (const auto& row : results) {
        models::User user;
        user.id = std::stoi(row.at("id"));
        user.name = row.at("name");
        user.age = std::stoi(row.at("age"));
        user.phoneNumber = row.at("phone_number");
        user.walletAddress = row.at("wallet_address");
        user.publicKey = row.at("public_key");
        users.push_back(user);
    }
    return users;
}

std::optional<models::User> DatabaseService::getUserByPublicKey(const std::string& pubKey) {
    std::string sql = "SELECT * FROM user WHERE public_key = '" + pubKey + "';";
    auto results = db_->query(sql);

    if (results.empty()) {
        return std::nullopt;
    }

    const auto& row = results[0];
    models::User user;
    user.id = std::stoi(row.at("id"));
    user.name = row.at("name");
    user.age = std::stoi(row.at("age"));
    user.phoneNumber = row.at("phone_number");
    user.walletAddress = row.at("wallet_address");
    user.publicKey = row.at("public_key");
    return user;
}

} // namespace services
