#include "PlayerConfig.h"
#include "ll/api/io/FileUtils.h"
#include "mod/FastMiner.h"
#include "utils/JsonUtils.h"
#include <filesystem>


namespace fm::PlayerConfig {

constexpr auto PLAYER_CONFIG_FILE = "PlayerSetting.json";

void load() {
    auto& mod  = FastMiner::getInstance().getSelf();
    auto  path = mod.getModDir() / PLAYER_CONFIG_FILE;

    if (!std::filesystem::exists(path)) {
        save();
        return;
    }

    auto content = ll::file_utils::readFile(path);
    if (!content) {
        throw std::runtime_error("Failed to read player config file");
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(*content);
    } catch (nlohmann::json::parse_error const& e) {
        throw std::runtime_error("Failed to parse player config file: " + std::string(e.what()));
    }

    json_utils::json2struct(cfg, json);
}

void save() {
    auto& mod  = FastMiner::getInstance().getSelf();
    auto  path = mod.getModDir() / PLAYER_CONFIG_FILE;

    auto json = ::json_utils::struct2json(cfg);
    ll::file_utils::writeFile(path, json.dump());
}

bool isEnabled(mce::UUID const& uuid, std::string const& key) {
    auto iter = cfg.find(uuid);
    if (iter == cfg.end()) {
        return false;
    }
    return iter->second[key];
}

void setEnabled(mce::UUID const& uuid, std::string const& key, bool enabled) {
    auto iter = cfg.find(uuid);
    if (iter == cfg.end()) {
        cfg[uuid] = {
            {key, enabled}
        };
        return;
    }
    iter->second[key] = enabled;
}

void enable(mce::UUID const& uuid, std::string const& key) { setEnabled(uuid, key, true); }

void disable(mce::UUID const& uuid, std::string const& key) { setEnabled(uuid, key, false); }


} // namespace fm::PlayerConfig