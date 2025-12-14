#pragma once
#include <string>
#include <vector>
#include <optional>

namespace utils {

    class SolanaUtils {
    public:
        // Reads a JSON keypair file and returns the public key as a byte vector.
        // Returns std::nullopt if the file is invalid or not a valid keypair.
        static std::optional<std::vector<uint8_t>> ReadPublicKeyFromKeypairFile(const std::string& filePath);
    };

}
