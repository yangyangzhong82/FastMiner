#include "config/Config.h"
#include "ll/api/Config.h"
#include "mod/FastMiner.h"
#include <filesystem>


namespace fm::Config {


void load() {
    auto path = FastMiner::getInstance().getSelf().getModDir() / "Config.json";

    if (!std::filesystem::exists(path)) {
        save();
        return;
    }

    bool ok = ll::config::loadConfig(cfg, path);
    if (!ok) {
        save();
    }
}

void save() {
    auto path = FastMiner::getInstance().getSelf().getModDir() / "Config.json";

    ll::config::saveConfig(cfg, path);
}


} // namespace fm::Config