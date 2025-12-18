#include "SolanaUtils.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace utils {

    std::optional<std::vector<uint8_t>> SolanaUtils::ReadPublicKeyFromKeypairFile(const std::string& filePath) {
        auto keypair = ReadKeypairFromKeypairFile(filePath);
        if (!keypair) {
            return std::nullopt;
        }

        // Extract the last 32 bytes (public key)
        std::vector<uint8_t> publicKey(keypair->begin() + 32, keypair->end());
        return publicKey;
    }

    std::optional<std::vector<uint8_t>> SolanaUtils::ReadKeypairFromKeypairFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return std::nullopt;
        }

        try {
            json j;
            file >> j;

            if (!j.is_array()) {
                return std::nullopt;
            }

            std::vector<uint8_t> keypair = j.get<std::vector<uint8_t>>();

            // Solana keypairs are 64 bytes: 32 bytes private key + 32 bytes public key.
            if (keypair.size() != 64) {
                return std::nullopt;
            }

            return keypair;

        } catch (const std::exception& e) {
            return std::nullopt;
        }
    }

}
