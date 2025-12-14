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

} // namespace infra::database
