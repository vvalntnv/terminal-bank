#pragma once

#include <string>
#include <sqlite3.h>

namespace infra::database {

class SQLiteWrapper {
public:
    SQLiteWrapper(const std::string& dbPath);
    ~SQLiteWrapper();

    void execute(const std::string& query);
    // Helper for executing queries with callbacks if needed later, 
    // but for now execute is enough for initialization.

    sqlite3* getHandle() const { return db_; }

private:
    sqlite3* db_ = nullptr;
};

} // namespace infra::database
