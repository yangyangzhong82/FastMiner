#include "MinerTask.h"
#include "McUtils.h"
#include "config/Config.h"
#include "core/MinerDispatcher.h"
#include "core/MinerPermitAwaiter.h"
#include "core/MinerTaskContext.h"
#include "core/MinerUtil.h"
#include "economy/EconomySystem.h"

#include "ll/api/chrono/GameChrono.h"
#include "ll/api/coro/CoroTask.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/player/PlayerDestroyBlockEvent.h"
#include "ll/api/thread/ServerThreadExecutor.h"

#include "mc/server/ServerLevel.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/item/enchanting/EnchantUtils.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/ActorChangeContext.h"
#include "mc/world/level/block/Block.h"
#include "mc/world/level/block/BlockChangeContext.h"
#include "mc/world/level/block/registry/BlockTypeRegistry.h"

#include <cstddef>
#include <vector>


namespace fm {


inline static std::vector<std::tuple<int, int, int>> AdjacentDirections = {
    {1,  0,  0 },
    {-1, 0,  0 },
    {0,  1,  0 },
    {0,  -1, 0 },
    {0,  0,  1 },
    {0,  0,  -1}
};

inline static std::vector<std::tuple<int, int, int>> CubeDirections = {
    {1,  0,  0 },
    {-1, 0,  0 },
    {0,  1,  0 },
    {0,  -1, 0 },
    {0,  0,  1 },
    {0,  0,  -1},
    {1,  0,  1 },
    {-1, 0,  1 },
    {0,  1,  1 },
    {0,  -1, 1 },
    {1,  1,  1 },
    {-1, -1, 1 },
    {1,  -1, 1 },
    {-1, 1,  1 },
    {1,  0,  -1},
    {-1, 0,  -1},
    {0,  1,  -1},
    {0,  -1, -1},
    {1,  1,  -1},
    {-1, -1, -1},
    {1,  -1, -1},
    {-1, 1,  -1},
    {1,  1,  0 },
    {-1, -1, 0 },
    {1,  -1, 0 },
    {-1, 1,  0 }
};


static TaskID next = 0;

MinerTask::MinerTask(MinerTaskContext ctx, MinerDispatcher& dispatcher)
: id(next++),
  player(ctx.player),
  tool(const_cast<ItemStack&>(player.getSelectedItem())),
  startBlock(ctx.block),
  startPosition(std::move(ctx.tiggerPos)),
  hashedStartPos(std::move(ctx.hashedPos)),
  blockType(startBlock.getTypeName()),
  blockConfig(ctx.blockConfig),
  blockSource(ctx.blockSource),
  limit(std::move(ctx.limit)),
  dimension(std::move(ctx.tiggerDimid)),
  durability(EnchantUtils::getEnchantLevel(::Enchant::Type::Unbreaking, tool)),
  directions(blockConfig.destroyMode == Config::DestroyMode::Cube ? CubeDirections : AdjacentDirections),
  dispatcher(dispatcher) {}

void MinerTask::execute() {
    queue.reserve(limit);
    visited.reserve(limit * 2);

    queue.emplace_back(startBlock, startPosition, hashedStartPos);
    visited.insert(hashedStartPos);
    state = State::Running;
    ll::coro::keepThis([this]() -> ll::coro::CoroTask<> {
        auto& bus = ll::event::EventBus::getInstance();

        auto awaiter       = MinerPermitAwaiter{this, dispatcher};
        auto ctx           = BlockChangeContext{};
        ctx.mContextSource = ActorChangeContext{.mActorContext = &player};

        auto totalCpuTime = std::chrono::milliseconds::zero(); // 总 CPU 耗时
        auto begin        = std::chrono::high_resolution_clock::now();

        size_t head = 0; // 队列头
        while (count < limit && head < queue.size() && canContinue()) {
            if (quota == 0) {
                auto end      = std::chrono::high_resolution_clock::now();
                totalCpuTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
                co_await awaiter;                                  // 等待许可额度
                if (!canContinue()) break;                         // 检查是否可以继续
                begin = std::chrono::high_resolution_clock::now(); // 重置开始时间
            }

            // 消费许可额度
            while (quota > 0 && canContinue()) {
                quota--;
                auto const& item            = queue[head++];
                auto const& [block, pos, _] = item;
                tryBreakBlock(item, bus, ctx);
                serachAdjacentBlocks(pos);
            }
        }
        auto end      = std::chrono::high_resolution_clock::now();
        totalCpuTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

        if (canContinue()) {
            notifyFinished(totalCpuTime.count());
        }

        co_return;
    }).launch(ll::thread::ServerThreadExecutor::getDefault());
}

void MinerTask::tryBreakBlock(QueueItem const& item, ll::event::EventBus& bus, BlockChangeContext ctx) {
    auto const& [block, pos, hashed] = item;
    if (block.isAir()) {
        return;
    }
    dispatcher.insertProcessing(hashed);

    auto event = ll::event::PlayerDestroyBlockEvent{player, pos};
    bus.publish(event);

    dispatcher.eraseProcessing(hashed);
    if (event.isCancelled()) {
        return;
    }

    count++; // 累计破坏方块数量
    block.playerDestroy(player, pos);
    // blockSource.removeBlock(pos, ctx);
    static auto& air = BlockTypeRegistry::get().getDefaultBlockState("minecraft:air");
    blockSource.setBlock(pos, air, 2, nullptr, ctx);
}


void MinerTask::calculateDurabilityDeduction() {
    if (durability == 0) {
        deductDamage = count; // 无耐久附魔，按照破坏数量扣除耐久
    } else {
        // TODO: 分离到其它线程进行计算?
        for (int i = 0; i < count; ++i) {
            if (MinerUtil::randomInt(0, 99) < 100 / (durability + 1)) deductDamage++;
        }
    }
}

void MinerTask::serachAdjacentBlocks(BlockPos const& pos) {
    for (auto const& [dx, dy, dz] : directions) {
        BlockPos adjacent{pos.x + dx, pos.y + dy, pos.z + dz};
        auto     hashed = MinerUtil::hashDimensionPosition(adjacent, dimension);
        if (visited.insert(hashed).second) {
            auto& block = blockSource.getBlock(adjacent);
            auto& type  = block.getTypeName();
            // TODO: 哈希优化 similarBlock 查找
            if (block == startBlock || blockConfig.similarBlock.contains(type)) {
                queue.emplace_back(block, std::move(adjacent), std::move(hashed));
            }
        }
    }
}

void MinerTask::notifyFinished(long long cpuTime) {
    ll::coro::keepThis([this, ms = std::move(cpuTime)]() -> ll::coro::CoroTask<> {
        co_await ll::chrono::ticks{1};
        calculateDurabilityDeduction();
        if (!MinerUtil::hasUnbreakable(tool)) {
            short damage = tool.getDamageValue() + deductDamage;
            tool.setDamageValue(damage);
            player.refreshInventory();
        }
        auto cost = blockConfig.cost * (count - 1);
        EconomySystem::getInstance()->reduce(player, cost);
        mc_utils::sendText(player, "本次连锁了 {} 个方块, 消耗了 {} 点耐久, 总耗时 {}ms", count, deductDamage, ms);
        notifyClientBlockUpdate();
        state = State::Finished;
        dispatcher.onTaskFinished(this);
        co_return;
    }).launch(ll::thread::ServerThreadExecutor::getDefault());
}

void MinerTask::notifyClientBlockUpdate() {
    // 任务已完成，可以把资源转移走
    ll::coro::keepThis([queue = std::move(queue), &bs = blockSource]() -> ll::coro::CoroTask<> {
        co_await ll::chrono::ticks{1};
        for (auto const& [block, pos, _] : queue) {
            block.neighborChanged(bs, pos, pos);
        }
        co_return;
    }).launch(ll::thread::ServerThreadExecutor::getDefault());
}

void MinerTask::interrupt() { state = State::Interrupted; }
bool MinerTask::isInterrupted() const { return state == State::Interrupted; }
bool MinerTask::isFinished() const { return state == State::Finished; }
bool MinerTask::isRunning() const { return state == State::Running; }
bool MinerTask::isPending() const { return state == State::Pending; }
bool MinerTask::canContinue() const { return state == State::Running || state == State::Pending; }

} // namespace fm