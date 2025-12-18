#include "RelayService.hpp"

namespace services {

RelayService::RelayService(std::shared_ptr<infra::RelayAPIClient> client)
    : client(std::move(client)) {}

std::string RelayService::CreateAccount(int id, const std::string &name,
                                        const std::string &pubKey,
                                        const std::string &privKey) {
  return client->InitializeAccount(id, name, pubKey, privKey);
}

std::string RelayService::AirdropSol(unsigned int amount,
                                     const std::string &pubKey,
                                     const std::string &privKey) {
  return client->AirdropSolana(amount, pubKey, privKey);
}

std::string RelayService::Deposit(int index, uint64_t amount,
                                  const std::string &pubKey,
                                  const std::string &privKey) {
  return client->Deposit(index, amount, pubKey, privKey);
}

std::string RelayService::Withdraw(int index, uint64_t amount,
                                   const std::string &pubKey,
                                   const std::string &privKey) {
  return client->Withdraw(index, amount, pubKey, privKey);
}

std::string RelayService::InternalTransfer(int fromIdx, int toIdx,
                                           uint64_t amount,
                                           const std::string &pubKey,
                                           const std::string &privKey) {
  return client->InternalTransfer(fromIdx, toIdx, amount, pubKey, privKey);
}

std::string RelayService::ExternalTransfer(int fromIdx,
                                           const std::string &toAddr,
                                           uint64_t amount,
                                           const std::string &pubKey,
                                           const std::string &privKey) {
  return client->ExternalTransfer(fromIdx, toAddr, amount, pubKey, privKey);
}

} // namespace services
