#pragma once
#include <string>
#include <optional>

namespace models {

    struct Account {
        int id = 0;
        int seedIndex = 0;
        std::string accountName; // Nullable in schema, using empty string for null
        std::string pubKey;
        std::string tokenSymbol = "LEV";
        std::string pdaPubKey;
        std::string ataPubKey;
    };

}
