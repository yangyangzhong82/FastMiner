#pragma once
#include "core/MinerLauncher.h"

namespace fm {
namespace client {

class ClientMinerLauncher final : public MinerLauncher {
public:
    bool isMinerEnabled(Player& player, const std::string& blockType) override;
    bool canDestroyBlockWithConfig(Player& player, const RuntimeBlockConfig::Ptr& rtConfig) override;
};

} // namespace client
} // namespace fm
