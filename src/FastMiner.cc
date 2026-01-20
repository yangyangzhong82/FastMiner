#include "FastMiner.h"
#include "command/FastMinerCommand.h"
#include "config/ConfigBase.h"
#include "config/ConfigFactory.h"

#include "ll/api/event/EventBus.h"
#include "ll/api/event/ListenerBase.h"
#include "ll/api/mod/RegisterHelper.h"

#include <memory>
#include <mutex>


#ifdef LL_PLAT_S
#include "config/ServerConfigModel.h"
#include "econbridge/detail/LegacyMoneyEconomy.h"
#include "econbridge/detail/NullEconomy.h"
#include "econbridge/detail/ScoreboardEconomy.h"
#endif

#ifdef LL_PLAT_C
#include "ll/api/event/client/ClientJoinLevelEvent.h"
#endif

#include "trait/MinerLauncherTrait.h"

namespace fm {

using LauncherImpl = internal::ImplType<tag::MinerLauncherTag>::type;

struct FastMiner::Impl {

    ll::mod::NativeMod&           mSelf;
    std::unique_ptr<LauncherImpl> mLauncher;

#ifdef LL_PLAT_S
    std::unique_ptr<econbridge::IEconomy> mEconomy{nullptr};

    void initEconomy() {
        if (!ConfigBase::data.economy.enabled) {
            mEconomy = std::make_unique<econbridge::detail::NullEconomy>();
        }
        switch (ConfigBase::data.economy.kit) {
        case ConfigBase::ConfigModel::EconomyConfig::EconomyKit::LegacyMoney:
            mEconomy = std::make_unique<econbridge::detail::LegacyMoneyEconomy>();
            break;
        case ConfigBase::ConfigModel::EconomyConfig::EconomyKit::ScoreBoard:
            mEconomy = std::make_unique<econbridge::detail::ScoreboardEconomy>(ConfigBase::data.economy.scoreboardName);
            break;
        }
    }
#endif

#ifdef LL_PLAT_C
    ll::event::ListenerPtr mClientJoinLevelListener{nullptr};
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
    auto& instance = ConfigFactory::getInstance();
    instance.buildDefaultConfig();
    instance.load();
    instance.buildRuntimeConfigMap();

#ifdef LL_PLAT_S
    FastMinerCommand::setup();

    mImpl->initEconomy();
#endif

    mImpl->mLauncher = std::make_unique<LauncherImpl>();

#ifdef LL_PLAT_C
    ll::event::EventBus::getInstance().emplaceListener<ll::event::ClientJoinLevelEvent>(
        [](ll::event::ClientJoinLevelEvent&) {
#ifdef DEBUG
            std::cout << "Client joined level, registering commands" << std::endl;
#endif
            FastMinerCommand::setup();
        }
    );
#endif

    return true;
}

bool FastMiner::disable() {
    ConfigFactory::getInstance().save();

    mImpl->mLauncher.reset();

#ifdef LL_PLAT_S
    mImpl->mEconomy.reset();
#endif

#ifdef LL_PLAT_C
    ll::event::EventBus::getInstance().removeListener(mImpl->mClientJoinLevelListener);
#endif

    return true;
}

bool FastMiner::unload() { return true; }

#ifdef LL_PLAT_S
econbridge::IEconomy& FastMiner::getEconomy() const { return *mImpl->mEconomy; }
#endif

} // namespace fm

LL_REGISTER_MOD(fm::FastMiner, fm::FastMiner::getInstance());
