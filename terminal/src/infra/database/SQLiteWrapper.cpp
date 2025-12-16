#include "SQLiteWrapper.hpp"
#include <iostream>
#include <stdexcept>

namespace infra::database {

SQLiteWrapper::SQLiteWrapper(const std::string& dbPath) {
    int rc = sqlite3_open(dbPath.c_str(), &db_);
    if (rc) {
        std::string errMsg = "Can't open database: " + std::string(sqlite3_errmsg(db_));
        sqlite3_close(db_);
        throw std::runtime_error(errMsg);
    }
}

SQLiteWrapper::~SQLiteWrapper() {
    if (db_) {
        sqlite3_close(db_);
    }
}

void SQLiteWrapper::execute(const std::string& query) {
    char* zErrMsg = 0;
    int rc = sqlite3_exec(db_, query.c_str(), nullptr, 0, &zErrMsg);
    
    if (rc != SQLITE_OK) {
        std::string errMsg = "SQL error: " + std::string(zErrMsg);
        sqlite3_free(zErrMsg);
        throw std::runtime_error(errMsg);
    }
}

std::vector<std::map<std::string, std::string>> SQLiteWrapper::query(const std::string& query) {
    std::vector<std::map<std::string, std::string>> results;
    char* zErrMsg = 0;
    
    auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
        auto* rows = static_cast<std::vector<std::map<std::string, std::string>>*>(data);
        std::map<std::string, std::string> row;
        for (int i = 0; i < argc; i++) {
            row[azColName[i]] = argv[i] ? argv[i] : "NULL";
        }
        rows->push_back(row);
        return 0;
    };

    int rc = sqlite3_exec(db_, query.c_str(), callback, &results, &zErrMsg);
    
    if (rc != SQLITE_OK) {
        std::string errMsg = "SQL query error: " + std::string(zErrMsg);
        sqlite3_free(zErrMsg);
        throw std::runtime_error(errMsg);
    }

    return results;
}

} // namespace infra::database
