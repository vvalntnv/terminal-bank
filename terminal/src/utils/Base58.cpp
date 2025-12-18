#include "Base58.hpp"
#include <vector>

namespace utils {

    static const char* pszBase58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

    std::string EncodeBase58(const std::vector<uint8_t>& input) {
        // Skip empty data
        if (input.empty())
            return "";

        // Count leading zeros
        int zeros = 0;
        while (zeros < input.size() && input[zeros] == 0)
            zeros++;

        // Allocate enough space in big-endian base58 representation
        // log(256) / log(58), rounded up.
        std::vector<unsigned char> b58((input.size() - zeros) * 138 / 100 + 1); 

        // Process the bytes
        int length = 0;
        for (size_t i = zeros; i < input.size(); i++) {
            int carry = input[i];
            for (std::vector<unsigned char>::reverse_iterator it = b58.rbegin(); it != b58.rend(); it++) {
                carry += 256 * (*it);
                *it = carry % 58;
                carry /= 58;
            }
            while (carry > 0) {
                b58.insert(b58.begin(), carry % 58);
                carry /= 58;
            }
        }

        // Skip leading zeros in b58 result
        std::vector<unsigned char>::iterator it = b58.begin();
        while (it != b58.end() && *it == 0)
            it++;

        // Translate the result into a string
        std::string str;
        str.reserve(zeros + (b58.end() - it));
        str.assign(zeros, '1');
        while (it != b58.end())
            str += pszBase58[*(it++)];
        
        return str;
    }

}
