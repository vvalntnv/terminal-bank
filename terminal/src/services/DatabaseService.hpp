#pragma once

#include "../infra/database/SQLiteWrapper.hpp"
#include <memory>
#include <string>

namespace services {

class DatabaseService {
public:
    DatabaseService(const std::string& dbPath);
    void init();

private:
    std::unique_ptr<infra::database::SQLiteWrapper> db_;
};

} // namespace services
