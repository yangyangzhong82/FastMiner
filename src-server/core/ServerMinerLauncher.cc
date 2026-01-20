#include "ServerMinerLauncher.h"

#include "FastMiner.h"
#include "config/ConfigFactory.h"
#include "config/ServerConfig.h"
#include "utils/McUtils.h"

#include <mc/world/item/enchanting/EnchantUtils.h>

namespace fm::server {

bool ServerMinerLauncher::isMinerEnabled(Player& player, const std::string& blockType) {
    if (!player.isSurvival()) {
        FM_TRACE("player not survival");
        return false;
    }

    auto& inst = ConfigFactory::getInstance().as<ServerConfig>();
    auto& uuid = player.getUuid();
    if (!inst.isEnabled(uuid, ServerConfig::KEY_ENABLE.data())) {
        FM_TRACE("player miner disabled");
        return false;
    }

    bool sneakingRequired = inst.isEnabled(uuid, ServerConfig::KEY_SNEAK.data());
    bool sneaking         = mc_utils::isSneaking(player);
    if (sneakingRequired && !sneaking) {
        FM_TRACE("sneaking required but player is not sneaking");
        return false;
    }
    return inst.isEnabled(uuid, blockType);
}

bool ServerMinerLauncher::canDestroyBlockWithConfig(Player& player, const RuntimeBlockConfig::Ptr& rtConfig) {
    const auto& config = rtConfig->rawConfig_;
    const auto& item   = player.getSelectedItem();

    if (!config.tools.empty() && !config.tools.contains(item.getTypeName())) {
        return false; // 限制了工具 && 工具不匹配
    }

    const bool hasSilkTouch = EnchantUtils::hasEnchant(Enchant::Type::SilkTouch, item);
    switch (config.silkTouchMode) {
    case SilkTouchMode::Unlimited:
        return true; // 不限制精准采集
    case SilkTouchMode::Forbid:
        return !hasSilkTouch; // 禁止精准采集
    case SilkTouchMode::Need:
        return hasSilkTouch; // 需要精准采集
    }
    return false;
}


int ServerMinerLauncher::calculateLimit(const MinerTaskContext& ctx) {
    int limit = MinerLauncher::calculateLimit(ctx);
    if (ServerConfig::data.economy.enabled && ctx.rtConfig->rawConfig_.cost > 0) {
        // 动态约束限制为玩家经济
        limit = std::min(
            limit,
            static_cast<int>(
                FastMiner::getInstance().getEconomy().get(ctx.player.getUuid()) / ctx.rtConfig->rawConfig_.cost
            )
        );
    }
    return limit;
}

} // namespace fm::server