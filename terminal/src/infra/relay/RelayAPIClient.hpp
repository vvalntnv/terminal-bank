#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace infra {

class RelayException : public std::runtime_error {
public:
    RelayException(const std::string& message) : std::runtime_error(message) {}
};

class RelayAPIClient {
public:
    explicit RelayAPIClient(const std::string& baseUrl);
    ~RelayAPIClient() = default;

    // POST /api/initialize-account
    std::string InitializeAccount(
        int userAccountId,
        const std::string& name,
        const std::string& pubKey,
        const std::string& privKey
    );

    // POST /api/deposit
    std::string Deposit(
        int index,
        uint64_t amount,
        const std::string& pubKey,
        const std::string& privKey
    );

    // POST /api/withdraw
    std::string Withdraw(
        int index,
        uint64_t amount,
        const std::string& pubKey,
        const std::string& privKey
    );

    // POST /api/transfer/internal
    std::string InternalTransfer(
        int senderIndex,
        int receiverIndex,
        uint64_t amount,
        const std::string& pubKey,
        const std::string& privKey
    );

    // POST /api/transfer/external
    std::string ExternalTransfer(
        int senderIndex,
        const std::string& recipient,
        uint64_t amount,
        const std::string& pubKey,
        const std::string& privKey
    );

private:
    std::string baseUrl;

    std::string makeRequest(
        const std::string& endpoint,
        const nlohmann::json& payload,
        const std::string& pubKey,
        const std::string& privKey
    );
};

} // namespace infra
