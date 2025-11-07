#pragma once
#include "Global.h"
#include "config/Config.h"
#include "core/MinerTaskContext.h"
#include "core/MinerUtil.h"

#include "absl/container/flat_hash_set.h"

#include "ll/api/event/EventBus.h"
#include "mc/deps/core/string/HashedString.h"
#include "mc/world/level/BlockPos.h"

#include <cstddef>
#include <cstdint>
#include <tuple>
#include <utility>
#include <vector>

class Player;
class ItemStack;
class Block;
class BlockPos;
class BlockSource;
class BlockChangeContext;

namespace fm {

class MinerDispatcher;

using TaskID = uint64_t;

/**
 * @brief 挖掘任务
 * 每个任务的承载单元
 */
struct MinerTask {
    enum class State {
        Pending,     // 待处理
        Running,     // 正在处理
        Finished,    // 处理完成
        Interrupted, // 被中断
    };

    TaskID const               id;                    // 任务id
    State                      state{State::Pending}; // 任务状态
    Player&                    player;                // 执行任务的玩家
    ItemStack&                 tool;                  // 使用的工具
    Block const&               startBlock;            // 任务起始方块
    BlockPos const             startPosition;         // 任务起始位置
    HashedDimPos const         hashedStartPos;        // 任务起始位置的哈希值
    HashedString const         blockType;             // 要挖掘的方块类型
    Config::BlockConfig const& blockConfig;           // 方块配置
    BlockSource&               blockSource;           // 方块源
    int const                  limit{0};              // 挖掘次数限制
    int const                  dimension;             // 任务所在的维度
    int const                  durability{0};         // 工具耐久度

    // BFS
    using QueueItem = std::tuple<Block const&, BlockPos, HashedDimPos>;
    std::vector<QueueItem>                        queue{};    // 任务队列
    absl::flat_hash_set<size_t>                   visited{};  // 已访问过的方块索引
    std::vector<std::tuple<int, int, int>> const& directions; // 方向
    MinerDispatcher&                              dispatcher; // 任务调度器

    // 计数
    int count{0};        // 挖掘次数
    int deductDamage{0}; // 扣除的耐久度
    int quota{0};        // 任务执行次数配额

    FM_DISABLE_COPY(MinerTask);
    using Ptr = std::shared_ptr<MinerTask>;

    explicit MinerTask(MinerTaskContext ctx, MinerDispatcher& dispatcher);

    void execute();
    void tryBreakBlock(QueueItem const& item, ll::event::EventBus& bus, BlockChangeContext ctx);
    void calculateDurabilityDeduction();
    void serachAdjacentBlocks(BlockPos const& pos);
    void notifyFinished(long long cpuTime);
    void notifyClientBlockUpdate(); // 通知客户端方块更新

    void interrupt();
    bool isInterrupted() const;
    bool isFinished() const;
    bool isRunning() const;
    bool isPending() const;
    bool canContinue() const;
};


} // namespace fm