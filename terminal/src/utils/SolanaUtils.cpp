#include "SolanaUtils.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <cmath>
#include <iomanip>

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

    uint64_t SolanaUtils::ParseTokenAmount(const std::string& amountStr) {
        try {
            double amount_d = std::stod(amountStr);
            if (amount_d <= 0) return 0;
            
            // Multiply by 10^9 and round
            return static_cast<uint64_t>(std::round(amount_d * 1000000000.0));
        } catch (...) {
            return 0;
        }
    }

}
