#include "mod/Mod.h"
#include "Command.h"
#include "Config.h"
#include "Core.h"
#include "ll/api/io/Logger.h"
#include "ll/api/mod/RegisterHelper.h"
#include <memory>


namespace fm {

Mod& Mod::getInstance() {
    static Mod instance;
    return instance;
}

bool Mod::load() {
    getSelf().getLogger().info("Loading...");

#ifdef DEBUG
    getSelf().getLogger().setLevel(ll::io::LogLevel::Debug);
#endif

    config::ConfImpl::load();
    config::ConfImpl::loadPlayerSetting();

    return true;
}

bool Mod::enable() {
    getSelf().getLogger().info("Enabling...");

    FastMinerCommand::setup();
    core::registerEvent();

    return true;
}

bool Mod::disable() {
    getSelf().getLogger().info("Disabling...");
    // Code for disabling the plugin goes here.
    return true;
}

} // namespace fm

LL_REGISTER_MOD(fm::Mod, fm::Mod::getInstance());
