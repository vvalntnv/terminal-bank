#include "DatabaseService.hpp"
#include <iostream>
#include <sstream>
#include "../models/Account.hpp"

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
  // Added 'balance' column
  std::string createAccountTable = "CREATE TABLE IF NOT EXISTS account ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "seed_index INTEGER NOT NULL,"
                                   "account_name TEXT,"
                                   "pub_key TEXT,"
                                   "token_symbol TEXT DEFAULT 'LEV',"
                                   "pda_pub_key TEXT,"
                                   "ata_pub_key TEXT,"
                                   "balance TEXT DEFAULT '0.00'"
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

void DatabaseService::createAccount(const models::Account& account) {
    std::string sql = "INSERT INTO account (seed_index, account_name, pub_key, token_symbol, pda_pub_key, ata_pub_key, balance) VALUES (" +
                      std::to_string(account.seedIndex) + ", '" +
                      account.accountName + "', '" +
                      account.pubKey + "', '" +
                      account.tokenSymbol + "', '" +
                      account.pdaPubKey + "', '" +
                      account.ataPubKey + "', '" +
                      account.balance + "');";
    db_->execute(sql);
}

int DatabaseService::getNextSeedIndex() {
    // Finds the maximum seed_index and returns max + 1
    std::string sql = "SELECT MAX(seed_index) as max_seed FROM account;";
    auto results = db_->query(sql);
    
    if (results.empty() || results[0]["max_seed"] == "NULL") {
        return 1;
    }
    
    return std::stoi(results[0]["max_seed"]) + 1;
}

std::vector<models::Account> DatabaseService::getAccountsForUser(const std::string& userPubKey) {
    std::string sql = "SELECT * FROM account WHERE pub_key = '" + userPubKey + "';";
    auto results = db_->query(sql);

    std::vector<models::Account> accounts;
    for (const auto& row : results) {
        models::Account acc;
        acc.id = std::stoi(row.at("id"));
        acc.seedIndex = std::stoi(row.at("seed_index"));
        acc.accountName = row.at("account_name");
        acc.pubKey = row.at("pub_key");
        acc.tokenSymbol = row.at("token_symbol");
        acc.pdaPubKey = row.at("pda_pub_key");
        acc.ataPubKey = row.at("ata_pub_key");
        // Handle balance field
        if (row.find("balance") != row.end()) {
            acc.balance = row.at("balance");
        } else {
            acc.balance = "0.00";
        }
        accounts.push_back(acc);
    }
    return accounts;
}

void DatabaseService::updateAccountBalance(int seedIndex, const std::string& balance) {
    std::string sql = "UPDATE account SET balance = '" + balance + "' WHERE seed_index = " + std::to_string(seedIndex) + ";";
    db_->execute(sql);
}

} // namespace services
