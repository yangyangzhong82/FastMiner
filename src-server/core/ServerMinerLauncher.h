#pragma once
#include "core/MinerLauncher.h"

namespace fm::server {

class ServerMinerLauncher final : public MinerLauncher {
public:
    bool isMinerEnabled(Player& player, const std::string& blockType) override;
    bool canDestroyBlockWithConfig(Player& player, const RuntimeBlockConfig::Ptr& rtConfig) override;
    int  calculateLimit(const MinerTaskContext& ctx) override;
};

}