#pragma once

#include <string>
#include <memory>
#include <cstdint>
#include "../infra/relay/RelayAPIClient.hpp"

namespace services {

class RelayService {
public:
    explicit RelayService(std::shared_ptr<infra::RelayAPIClient> client);

    std::string CreateAccount(int id, const std::string& name, const std::string& pubKey, const std::string& privKey);
    std::string Deposit(int index, uint64_t amount, const std::string& pubKey, const std::string& privKey);
    std::string Withdraw(int index, uint64_t amount, const std::string& pubKey, const std::string& privKey);
    std::string InternalTransfer(int fromIdx, int toIdx, uint64_t amount, const std::string& pubKey, const std::string& privKey);
    std::string ExternalTransfer(int fromIdx, const std::string& toAddr, uint64_t amount, const std::string& pubKey, const std::string& privKey);

private:
    std::shared_ptr<infra::RelayAPIClient> client;
};

} // namespace services
