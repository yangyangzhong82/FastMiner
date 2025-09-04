#include "economy/EconomySystem.h"
#include "config/Config.h"
#include "economy/impl/EmtpyInterface.h"
#include "economy/impl/LegacyMoneyInterface.h"
#include "economy/impl/ScoreBoardInterface.h"
#include "mod/FastMiner.h"
#include <memory>
#include <stdexcept>


namespace fm {


std::shared_ptr<internal::IEconomyInterface> EconomySystem::createEconomySystem() const {
    auto& cfg = getConfig();
    if (!cfg.enabled) {
        fm::FastMiner::getInstance().getSelf().getLogger().debug("using internal::EmtpyInterface");
        return std::make_shared<internal::EmtpyInterface>();
    }

    switch (cfg.kit) {
    case fm::EconomyKit::LegacyMoney: {
        fm::FastMiner::getInstance().getSelf().getLogger().debug("using internal::LegacyMoneyInterface");
        return std::make_shared<internal::LegacyMoneyInterface>();
    }
    case fm::EconomyKit::ScoreBoard: {
        fm::FastMiner::getInstance().getSelf().getLogger().debug("using internal::ScoreBoardInterface");
        return std::make_shared<internal::ScoreBoardInterface>();
    }
    }

    throw std::runtime_error("Unknown economy kit, please check config!");
}

EconomySystem& EconomySystem::getInstance() {
    static EconomySystem instance;
    return instance;
}

std::shared_ptr<internal::IEconomyInterface> EconomySystem::getEconomyInterface() const {
    std::lock_guard<std::mutex> lock(mInstanceMutex);
    if (!mEconomySystem) {
        throw std::runtime_error("internal::IEconomyInterface not initialized.");
    }
    return mEconomySystem;
}

EconomyConfig& EconomySystem::getConfig() const { return Config::cfg.economy; }

void EconomySystem::initEconomySystem() {
    std::lock_guard<std::mutex> lock(mInstanceMutex);
    if (!mEconomySystem) {
        mEconomySystem = createEconomySystem();
        return;
    }
}
void EconomySystem::reloadEconomySystem() {
    std::lock_guard<std::mutex> lock(mInstanceMutex);
    mEconomySystem = createEconomySystem();
}


EconomySystem::EconomySystem() = default;

std::shared_ptr<internal::IEconomyInterface> EconomySystem::operator->() const { return mEconomySystem; }


} // namespace fm
