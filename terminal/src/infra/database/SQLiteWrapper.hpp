#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sqlite3.h>

namespace infra::database {

class SQLiteWrapper {
public:
    SQLiteWrapper(const std::string& dbPath);
    ~SQLiteWrapper();

    void execute(const std::string& query);
    
    // Executes a query and returns a vector of rows, where each row is a map of column name -> value
    std::vector<std::map<std::string, std::string>> query(const std::string& query);

    sqlite3* getHandle() const { return db_; }

private:
    sqlite3* db_ = nullptr;
};

} // namespace infra::database
