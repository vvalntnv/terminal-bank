#pragma once
#include <string>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace utils {

class ConfigManager {
public:
    static std::string getConfigPath() {
        const char* home = std::getenv("HOME");
        if (!home) return "";
        return std::string(home) + "/.tbank_conf.json";
    }

    static std::string getKeysDirectory() {
        const char* home = std::getenv("HOME");
        if (!home) return "";
        std::string path = std::string(home) + "/.tbank_keys";
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directory(path);
        }
        return path;
    }

    static bool hasSession() {
        return std::filesystem::exists(getConfigPath());
    }

    static void saveSession(const std::string& keypairPath) {
        nlohmann::json j;
        j["keypair_path"] = keypairPath;
        
        std::ofstream file(getConfigPath());
        file << j.dump(4);
    }

    static std::string getSessionKeypairPath() {
        if (!hasSession()) return "";
        try {
            std::ifstream file(getConfigPath());
            nlohmann::json j;
            file >> j;
            return j.value("keypair_path", "");
        } catch (...) {
            return "";
        }
    }

    static void clearSession() {
        std::filesystem::remove(getConfigPath());
    }
};

}
