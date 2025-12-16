#include "RelayAPIClient.hpp"
#include <cpr/cpr.h>
#include <iostream>

namespace infra {

RelayAPIClient::RelayAPIClient(const std::string& baseUrl) : baseUrl(baseUrl) {}

std::string RelayAPIClient::InitializeAccount(
    int userAccountId,
    const std::string& name,
    const std::string& pubKey,
    const std::string& privKey
) {
    nlohmann::json payload = {
        {"userAccountId", userAccountId},
        {"userAccountName", name}
    };
    return makeRequest("/api/initialize-account", payload, pubKey, privKey);
}

std::string RelayAPIClient::Deposit(
    int index,
    uint64_t amount,
    const std::string& pubKey,
    const std::string& privKey
) {
    nlohmann::json payload = {
        {"index", index},
        {"amount", std::to_string(amount)} // Send as string to preserve u64 precision
    };
    return makeRequest("/api/deposit", payload, pubKey, privKey);
}

std::string RelayAPIClient::Withdraw(
    int index,
    uint64_t amount,
    const std::string& pubKey,
    const std::string& privKey
) {
    nlohmann::json payload = {
        {"index", index},
        {"amount", std::to_string(amount)}
    };
    return makeRequest("/api/withdraw", payload, pubKey, privKey);
}

std::string RelayAPIClient::InternalTransfer(
    int senderIndex,
    int receiverIndex,
    uint64_t amount,
    const std::string& pubKey,
    const std::string& privKey
) {
    nlohmann::json payload = {
        {"senderIndex", senderIndex},
        {"receiverIndex", receiverIndex},
        {"amount", std::to_string(amount)}
    };
    return makeRequest("/api/transfer/internal", payload, pubKey, privKey);
}

std::string RelayAPIClient::ExternalTransfer(
    int senderIndex,
    const std::string& recipient,
    uint64_t amount,
    const std::string& pubKey,
    const std::string& privKey
) {
    nlohmann::json payload = {
        {"senderIndex", senderIndex},
        {"to", recipient},
        {"amount", std::to_string(amount)}
    };
    return makeRequest("/api/transfer/external", payload, pubKey, privKey);
}

std::string RelayAPIClient::makeRequest(
    const std::string& endpoint,
    const nlohmann::json& payload,
    const std::string& pubKey,
    const std::string& privKey
) {
    std::string url = baseUrl + endpoint;
    
    cpr::Response r = cpr::Post(
        cpr::Url{url},
        cpr::Header{
            {"Content-Type", "application/json"},
            {"X-PUB-KEY", pubKey},
            {"X-PRIV-KEY", privKey}
        },
        cpr::Body{payload.dump()}
    );

    if (r.status_code >= 200 && r.status_code < 300) {
        try {
            auto jsonResponse = nlohmann::json::parse(r.text);
            if (jsonResponse.contains("signature")) {
                return jsonResponse["signature"].get<std::string>();
            }
            return "Success";
        } catch (...) {
            return r.text;
        }
    } else {
        // Try to parse error message from JSON
        std::string errorMessage = "Unknown error";
        try {
            auto jsonResponse = nlohmann::json::parse(r.text);
            if (jsonResponse.contains("message")) {
                errorMessage = jsonResponse["message"].get<std::string>();
            } else if (jsonResponse.contains("error")) {
                errorMessage = jsonResponse["error"].get<std::string>();
            } else {
                errorMessage = r.text;
            }
        } catch (...) {
            errorMessage = "HTTP Error " + std::to_string(r.status_code) + ": " + r.text;
        }
        throw RelayException(errorMessage);
    }
}

} // namespace infra
