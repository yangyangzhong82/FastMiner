#include "ClientMinerLauncher.h"

namespace fm {
namespace client {


bool ClientMinerLauncher::isMinerEnabled(Player& player, const std::string& blockType) {
    return true; // TODO: impl
}

bool ClientMinerLauncher::canDestroyBlockWithConfig(Player& player, const RuntimeBlockConfig::Ptr& rtConfig) {
    return true; // TODO: impl
}


} // namespace client
} // namespace fm