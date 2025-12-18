#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace utils {
    // Encodes a byte array into a Base58 string.
    std::string EncodeBase58(const std::vector<uint8_t>& input);
}
