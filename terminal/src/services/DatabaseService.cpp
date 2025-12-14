#include "DatabaseService.hpp"

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
                                "public_key TEXT NOT NULL"
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

} // namespace services
