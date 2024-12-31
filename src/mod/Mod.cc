#include "mod/Mod.h"

#include <memory>

#include "command/Command.h"
#include "config/Config.h"
#include "core/Core.h"
#include "ll/api/io/Logger.h"
#include "ll/api/mod/RegisterHelper.h"


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

    utils::Moneys::getInstance().updateConfig(config::ConfImpl::cfg.moneys);

    return true;
}

bool Mod::enable() {
    getSelf().getLogger().info("Enabling...");

    command::registerCommand();
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
