#include "mod/FastMiner.h"
#include "command/FastMinerCommand.h"
#include "config/Config.h"
#include "config/PlayerConfig.h"
#include "core/MinerLauncher.h"

#include "ll/api/mod/RegisterHelper.h"

#include <memory>


#ifdef LL_PLAT_S
#include "econbridge/detail/LegacyMoneyEconomy.h"
#include "econbridge/detail/NullEconomy.h"
#include "econbridge/detail/ScoreboardEconomy.h"
#endif


namespace fm {
using Config::cfg;


struct FastMiner::Impl {
    ll::mod::NativeMod&            mSelf;
    std::unique_ptr<MinerLauncher> mLauncher;

#ifdef LL_PLAT_S
    std::unique_ptr<econbridge::IEconomy> mEconomy{nullptr};

    void initEconomy() {
        if (!Config::cfg.economy.enabled) {
            mEconomy = std::make_unique<econbridge::detail::NullEconomy>();
        }
        switch (cfg.economy.kit) {
        case Config::Impl::EconomyConfig::EconomyKit::LegacyMoney:
            mEconomy = std::make_unique<econbridge::detail::LegacyMoneyEconomy>();
            break;
        case Config::Impl::EconomyConfig::EconomyKit::ScoreBoard:
            mEconomy = std::make_unique<econbridge::detail::ScoreboardEconomy>(Config::cfg.economy.scoreboardName);
            break;
        }
    }
#endif

    Impl() : mSelf(*ll::mod::NativeMod::current()) {}
};

FastMiner::FastMiner() : mImpl(std::make_unique<Impl>()) {}

ll::mod::NativeMod& FastMiner::getSelf() { return mImpl->mSelf; }

FastMiner& FastMiner::getInstance() {
    static FastMiner instance;
    return instance;
}

bool FastMiner::load() { return true; }

bool FastMiner::enable() {
    Config::load();

    PlayerConfig::load();
    PlayerConfig::checkAndTryRemoveNotExistBlock();

    FastMinerCommand::setup();

#ifdef LL_PLAT_S
    mImpl->initEconomy();
#endif

    mImpl->mLauncher = std::make_unique<MinerLauncher>();

    return true;
}

bool FastMiner::disable() {
    PlayerConfig::save();

    mImpl->mLauncher.reset();

#ifdef LL_PLAT_S
    mImpl->mEconomy.reset();
#endif

    return true;
}

bool FastMiner::unload() { return true; }

#ifdef LL_PLAT_S
econbridge::IEconomy& FastMiner::getEconomy() const { return *mImpl->mEconomy; }
#endif

} // namespace fm

LL_REGISTER_MOD(fm::FastMiner, fm::FastMiner::getInstance());
