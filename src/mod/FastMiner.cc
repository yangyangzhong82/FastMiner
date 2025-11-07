#include "mod/FastMiner.h"
#include "command/FastMinerCommand.h"
#include "config/Config.h"
#include "config/PlayerConfig.h"
#include "economy/EconomySystem.h"
#include "ll/api/mod/RegisterHelper.h"
#include <memory>


namespace fm {

FastMiner& FastMiner::getInstance() {
    static FastMiner instance;
    return instance;
}

bool FastMiner::load() { return true; }

bool FastMiner::enable() {
    Config::buildDefaultConfig();
    Config::load();

    PlayerConfig::load();
    PlayerConfig::checkAndTryRemoveNotExistBlock();

    EconomySystem::getInstance().initEconomySystem();

    FastMinerCommand::setup();

    mLauncher = std::make_unique<MinerLauncher>();

    return true;
}

bool FastMiner::disable() {
    PlayerConfig::save();

    mLauncher.reset();

    return true;
}

bool FastMiner::unload() { return true; }

} // namespace fm

LL_REGISTER_MOD(fm::FastMiner, fm::FastMiner::getInstance());
