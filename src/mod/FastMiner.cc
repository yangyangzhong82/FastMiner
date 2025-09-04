#include "mod/FastMiner.h"
#include "command/FastMinerCommand.h"
#include "config/Config.h"
#include "config/PlayerConfig.h"
#include "core/Core.h"
#include "economy/EconomySystem.h"
#include "ll/api/io/Logger.h"
#include "ll/api/mod/RegisterHelper.h"
#include <memory>


namespace fm {

FastMiner& FastMiner::getInstance() {
    static FastMiner instance;
    return instance;
}

bool FastMiner::load() {
    auto& logger = getSelf().getLogger();

#ifdef DEBUG
    logger.setLevel(ll::io::LogLevel::Debug);
#endif

    Config::load();
    PlayerConfig::load();

    return true;
}

bool FastMiner::enable() {
    EconomySystem::getInstance().initEconomySystem();

    FastMinerCommand::setup();
    core::registerEvent();

    return true;
}

bool FastMiner::disable() {
    PlayerConfig::save();

    return true;
}

bool FastMiner::unload() { return true; }

} // namespace fm

LL_REGISTER_MOD(fm::FastMiner, fm::FastMiner::getInstance());
