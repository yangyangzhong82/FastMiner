#pragma once
#include "config/Config.h"
#include "core/MinerUtil.h"

#include "mc/world/level/BlockPos.h"

class Player;
class BlockSource;

namespace fm {

/**
 * @brief DTO 对象，用于辅助传参
 */
struct MinerTaskContext {
    Player&                    player;      // 玩家对象
    BlockPos                   tiggerPos;   // 触发位置
    int                        tiggerDimid; // 触发维度
    HashedDimPos               hashedPos;   // 触发位置的哈希值
    Block const&               block;       // 触发的方块
    BlockSource&               blockSource; // 方块源
    Config::BlockConfig const& blockConfig; // 方块配置

    int limit{0};
};


} // namespace fm