#include "ServerPlatformService.h"

#include "command/FastMinerCommand.h"
#include "config/ServerConfig.h"
#include "config/ServerConfigModel.h"

#include "econbridge/detail/LegacyMoneyEconomy.h"
#include "econbridge/detail/NullEconomy.h"
#include "econbridge/detail/ScoreboardEconomy.h"

namespace fm {
namespace server {

struct ServerPlatformService::Impl {
    std::unique_ptr<econbridge::IEconomy> mEconomy{nullptr};

    void initEconomy() {
        if (!ServerConfig::data.economy.enabled) {
            mEconomy = std::make_unique<econbridge::detail::NullEconomy>();
        }
        switch (ServerConfig::data.economy.kit) {
        case ServerConfig::ConfigModel::EconomyConfig::EconomyKit::LegacyMoney:
            mEconomy = std::make_unique<econbridge::detail::LegacyMoneyEconomy>();
            break;
        case ServerConfig::ConfigModel::EconomyConfig::EconomyKit::ScoreBoard:
            mEconomy =
                std::make_unique<econbridge::detail::ScoreboardEconomy>(ServerConfig::data.economy.scoreboardName);
            break;
        }
    }
};

ServerPlatformService::ServerPlatformService() : impl(std::make_unique<Impl>()) {}

ServerPlatformService::~ServerPlatformService() = default;

bool ServerPlatformService::init() {
    FastMinerCommand::setup();
    impl->initEconomy();
    return true;
}

bool ServerPlatformService::destroy() {
    impl->mEconomy.reset();
    return true;
}

econbridge::IEconomy& ServerPlatformService::getEconomy() const { return *impl->mEconomy; }


} // namespace server
} // namespace fm