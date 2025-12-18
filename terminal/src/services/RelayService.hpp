#pragma once

#include "../infra/relay/RelayAPIClient.hpp"
#include <cstdint>
#include <memory>
#include <string>

namespace services {

class RelayService {
public:
  explicit RelayService(std::shared_ptr<infra::RelayAPIClient> client);

  std::string AirdropSol(unsigned int amount, const std::string &pubKey,
                         const std::string &privKey);

  std::string CreateAccount(int id, const std::string &name,
                            const std::string &pubKey,
                            const std::string &privKey);

  std::string Deposit(int index, uint64_t amount, const std::string &pubKey,
                      const std::string &privKey);

  std::string Withdraw(int index, uint64_t amount, const std::string &pubKey,
                       const std::string &privKey);

  std::string InternalTransfer(int fromIdx, int toIdx, uint64_t amount,
                               const std::string &pubKey,
                               const std::string &privKey);

  std::string ExternalTransfer(int fromIdx, const std::string &toAddr,
                               uint64_t amount, const std::string &pubKey,
                               const std::string &privKey);

private:
  std::shared_ptr<infra::RelayAPIClient> client;
};

} // namespace services
