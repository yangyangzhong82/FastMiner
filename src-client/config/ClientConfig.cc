#include "ClientConfig.h"

#include "FastMiner.h"

#include "ll/api/Config.h"

#include <filesystem>

namespace fm::client {

constexpr std::string_view FileName = "Config.json";

void ClientConfig::load() {
    namespace fs = std::filesystem;

    auto path = FastMiner::getInstance().getSelf().getConfigDir() / FileName;

    if (!fs::exists(path) || !ll::config::loadConfig(data, path)) {
        save();
    }

    buildRuntimeConfigMap();
}

void ClientConfig::save() {
    auto path = FastMiner::getInstance().getSelf().getConfigDir() / FileName;
    ll::config::saveConfig(data, path);
}

void ClientConfig::buildRuntimeConfigMap() {
    // TODO
}


} // namespace fm::client