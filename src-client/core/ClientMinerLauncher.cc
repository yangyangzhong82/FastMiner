#include "ClientMinerLauncher.h"

#include "FastMiner.h"

namespace fm {
namespace client {


bool ClientMinerLauncher::isMinerEnabled(Player& player, const std::string& blockType) {
    return FastMiner::getInstance().getPlatformService().as<ClientPlatformService>().isKeyActivated();
}

bool ClientMinerLauncher::canDestroyBlockWithConfig(Player& player, const RuntimeBlockConfig::Ptr& rtConfig) {
    return true;
}


} // namespace client
} // namespace fm