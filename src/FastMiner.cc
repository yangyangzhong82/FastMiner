#include "FastMiner.h"
#include "command/FastMinerCommand.h"
#include "config/ConfigBase.h"
#include "config/ConfigFactory.h"

#include "ll/api/mod/RegisterHelper.h"

#include <memory>

#include "trait/MinerLauncherTrait.h"

namespace fm {

using LauncherImpl = internal::ImplType<tag::MinerLauncherTag>::type;

struct FastMiner::Impl {
    ll::mod::NativeMod&                  mSelf;
    std::unique_ptr<LauncherImpl>        mLauncher{nullptr};
    std::unique_ptr<PlatformServiceImpl> mPlatformService{nullptr};

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

    mImpl->mPlatformService = std::make_unique<PlatformServiceImpl>();
    mImpl->mPlatformService->init();

    mImpl->mLauncher = std::make_unique<LauncherImpl>();

    return true;
}

bool FastMiner::disable() {
    ConfigFactory::getInstance().save();

    mImpl->mLauncher.reset();
    mImpl->mPlatformService->destroy();
    mImpl->mPlatformService.reset();

    return true;
}

bool FastMiner::unload() { return true; }

FastMiner::PlatformServiceImpl& FastMiner::getPlatformService() const { return *mImpl->mPlatformService; }

} // namespace fm

LL_REGISTER_MOD(fm::FastMiner, fm::FastMiner::getInstance());
