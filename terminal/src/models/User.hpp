#pragma once
#include <string>
#include <optional>

namespace models {

    struct User {
        int id = 0;
        std::string name;
        int age = 0;
        std::string phoneNumber;
        std::string walletAddress;
        std::string publicKey;
    };

}
