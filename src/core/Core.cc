#include "core/Core.h"
#include "EconomySystem.h"
#include "McUtils.h"
#include "config/PlayerConfig.h"
#include "ll/api/base/StdInt.h"
#include "ll/api/coro/Collect.h"
#include "ll/api/coro/CoroTask.h"
#include "ll/api/coro/Executor.h"
#include "ll/api/coro/ForwardAwaiter.h"
#include "ll/api/coro/Generator.h"
#include "ll/api/coro/YieldAwaiter.h"
#include "ll/api/thread/ServerThreadExecutor.h"
#include "mc/nbt/ByteTag.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/item/Item.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/item/ItemStackBase.h"
#include "mc/world/item/SaveContext.h"
#include "mc/world/item/SaveContextFactory.h"
#include <algorithm>
#include <tuple>
#include <vector>


#define logger fm::FastMiner::getInstance().getSelf().getLogger()


namespace core = fm::core;
using namespace core;


struct TaskInfo {
    std::string mBlockTypeName;   // 方块命名空间
    int         mDimension;       // 维度
    int         mLimit;           // 最大数量
    int         mCount;           // 当前数量
    int         mDeductDamage;    // 扣除耐久
    int         mDurabilityLevel; // 耐久等级
    ItemStack*  mTool;            // 工具
    Player*     mPlayer;          // 玩家
};


ll::event::ListenerPtr _mPlayerDestroyBlock;

std::unordered_map<int, TaskInfo> mTaskList;    // 任务列表
std::unordered_set<size_t>        mRuningBlock; // 正在执行任务的方块

void nextTick(std::function<void()> func) {
    using ll::chrono_literals::operator""_tick;
    ll::coro::keepThis([func{std::move(func)}]() -> ll::coro::CoroTask<> {
        co_await 1_tick;
        logger.debug("111");
        func();
        co_return;
    }).launch(ll::thread::ServerThreadExecutor::getDefault());
}

size_t core::hash(const BlockPos& v3, const int& dim) {
    std::hash<int> hasf;
    return hasf(v3.x) ^ hasf(v3.y) ^ hasf(v3.z) ^ hasf(dim);
}
size_t core::hash(ll::event::player::PlayerDestroyBlockEvent const& ev) {
    return core::hash(ev.pos(), ev.self().getDimensionId().id);
}
int core::randomInt() {
    static std::random_device                 rd;
    static std::default_random_engine         e(rd());
    static std::uniform_int_distribution<int> dist(0, 99);
    return dist(e);
}


inline bool hasUnbreakable(ItemStack* item) {
    auto nbt = item->save(*SaveContextFactory::createCloneSaveContext());
    if (!nbt->contains("tag")) return false;
    auto& tag = (*nbt)["tag"];
    if (tag.contains("Unbreakable")) {
        return tag["Unbreakable"].get<ByteTag>();
    }
    return false;
}


// Core
void core::unRegisterEvent() { ll::event::EventBus::getInstance().removeListener(_mPlayerDestroyBlock); }
void core::registerEvent() {
    mRuningBlock.reserve(128); // 预分配

    _mPlayerDestroyBlock =
        ll::event::EventBus::getInstance().emplaceListener<ll::event::player::PlayerDestroyBlockEvent>(
            [](ll::event::player::PlayerDestroyBlockEvent& ev) {
                if (mRuningBlock.contains(hash(ev)) || ev.isCancelled()) return; // 任务正在执行

#ifdef DEBUG
                logger.warn("DestroyBlock");
#endif

                Player*            player   = &ev.self();
                const Block*       block    = &player->getDimensionBlockSource().getBlock(ev.pos());
                std::string const& typeName = block->getTypeName();

                if (player->getPlayerGameType() != GameType::Survival ||                // 非生存模式
                    !PlayerConfig::isEnabled(player->getUuid().asString(), "enable") || // 未启用
                    !PlayerConfig::isEnabled(player->getUuid().asString(), typeName) || // 方块未启用
                    (PlayerConfig::isEnabled(player->getUuid().asString(), "sneak")
                     && !mc_utils::PlayerIsSneaking(*player)) // 未潜行
                ) {
                    return;
                }

#ifdef DEBUG
                logger.warn("Prepare Task");
#endif

                BlockPos blockPos = BlockPos(ev.pos());

                nextTick([player, block, blockPos, typeName]() {
                    if (!player->getDimensionBlockSourceConst().getBlock(blockPos).isAir()) return; // 被拦截

                    auto const& iter = Config::cfg.blocks.find(typeName);

#ifdef DEBUG
                    logger.warn("Finding block...");
#endif

                    if (iter == Config::cfg.blocks.end()) return; // 方块未配置
                    auto const&        confBlock = iter->second;
                    auto*              tool      = const_cast<ItemStack*>(&player->getSelectedItem()); // 工具
                    std::string const& toolType  = tool->getTypeName();                                // 工具命名空间

                    // clang-format off
                    bool const hasSilkTouch = EnchantUtils::hasEnchant(Enchant::Type::SilkTouch, *tool);

                    bool const canDestroyWithAPI =  mc_utils::CanDestroyBlock(*tool,*block) || mc_utils::CanDestroySpecial(*tool,*block);
                    bool const canDestroyWithConfig   = 
                        (confBlock.tools.empty() || confBlock.tools.contains(toolType)) && // 未指定工具、指定工具
                        (
                            (confBlock.silkTouchMod == Config::SilkTouchMode::Forbid && !hasSilkTouch) || // 禁止精准
                            (confBlock.silkTouchMod == Config::SilkTouchMode::Need && hasSilkTouch) || // 需要精准
                            confBlock.silkTouchMod == Config::SilkTouchMode::Unlimited // 无限制
                        );
            // clang-format on

#ifdef DEBUG
                    logger.warn(
                        "canDestroyWithConfig: {}  |  canDestroyWithAPI： {}",
                        canDestroyWithConfig,
                        canDestroyWithAPI
                    );
#endif
                    if (!canDestroyWithConfig || !canDestroyWithAPI) return;


                    int maxLimit = confBlock.limit; // 最大挖掘数量
                    if (!hasUnbreakable(tool)) {
                        if (tool->isDamageableItem()) {
                            maxLimit = std::min(maxLimit, (tool->getMaxDamage() - tool->getDamageValue() - 1));

                            if (Config::cfg.economy.enabled && confBlock.cost != 0) {
                                maxLimit = std::min(
                                    maxLimit,
                                    static_cast<int>(EconomySystem::getInstance().get(*player) / confBlock.cost)
                                );
                            }
                        }
                    }

                    if (maxLimit > 1) {
                        int const id = static_cast<int>(mTaskList.size()) + 1;
                        mTaskList.emplace(
                            id,
                            TaskInfo{
                                typeName,
                                player->getDimensionId().id,
                                maxLimit,
                                0,
                                0,
                                EnchantUtils::getEnchantLevel(::Enchant::Type::Unbreaking, *tool), // 耐久
                                tool,
                                player
                            }
                        );
                        miner(id, blockPos); // 执行任务
#ifdef DEBUG
                        logger.warn("Run Task");
#endif
                    }
                });
            }
        );
}


void core::miner(const int& taskID, const BlockPos stratPos) {
    static std::vector<std::tuple<int, int, int>> _dirDefault = {
        {1,  0,  0 },
        {-1, 0,  0 },
        {0,  1,  0 },
        {0,  -1, 0 },
        {0,  0,  1 },
        {0,  0,  -1}
    };
    // 3x3x3
    static std::vector<std::tuple<int, int, int>> _dirCube = {
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
    try {
        auto&       task      = mTaskList[taskID];
        auto const& confBlock = Config::cfg.blocks[task.mBlockTypeName];

        auto& dirs = confBlock.destroyMod == Config::DestroyMode::Cube ? _dirCube : _dirDefault;

        BlockSource& bs  = task.mPlayer->getDimensionBlockSource();
        auto&        bus = ll::event::EventBus::getInstance();

        std::vector<std::pair<const Block*, BlockPos>> mQueue;
        std::unordered_set<size_t>                     mVisited;

        mQueue.reserve(task.mLimit); // 预分配空间
        mVisited.reserve(task.mLimit * 2);

        size_t startHash = hash(stratPos, task.mDimension);
        mQueue.emplace_back(&bs.getBlock(stratPos), stratPos);
        mVisited.insert(startHash);


        auto   start      = std::chrono::high_resolution_clock::now();
        size_t queueIndex = 0;
        while (task.mCount < task.mLimit && queueIndex < mQueue.size()) {
            auto const& [curBlock, curPos] = mQueue[queueIndex++];

            // 处理
            if (!curBlock->isAir()) {
                auto const curHashed = hash(curPos, task.mDimension);
                mRuningBlock.insert(curHashed);

                auto ev = ll::event::player::PlayerDestroyBlockEvent(*task.mPlayer, curPos);
                bus.publish(ev);

                mRuningBlock.erase(curHashed);

                if (!ev.isCancelled()) {
                    curBlock->playerDestroy(*task.mPlayer, curPos);
                    bs.removeBlock(curPos);
                    task.mCount++;
                    if (task.mDurabilityLevel == 0
                        || (task.mDurabilityLevel > 0 && randomInt() < (100 / task.mDurabilityLevel + 1))) {
                        task.mDeductDamage++;
                    }
                }
            }

            // BFS 搜索
            for (auto const& [x, y, z] : dirs) {
                BlockPos nextPos(curPos.x + x, curPos.y + y, curPos.z + z);
                size_t   hashed = hash(nextPos, task.mDimension);

                if (mVisited.insert(hashed).second) { // 如果插入成功（即之前未访问过）
                    const Block*       nextBlock    = &bs.getBlock(nextPos);
                    std::string const& nextTypeName = nextBlock->getTypeName();

                    if (task.mBlockTypeName == nextTypeName || confBlock.similarBlock.contains(nextTypeName)) {
                        mQueue.emplace_back(nextBlock, std::move(nextPos));
                    }
                }
            }
        }
        auto end      = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);


        // 计算结果
        nextTick([taskID, task, confBlock, duration]() {
            if (task.mCount > 0) {
                auto* pl = task.mPlayer;

                auto* tool = task.mTool;
                if (!hasUnbreakable(tool)) {
                    short dmg = tool->getDamageValue() + task.mDeductDamage;
                    tool->setDamageValue(dmg);
                }

                auto cost = confBlock.cost * (task.mCount - 1);
                EconomySystem::getInstance().reduce(*pl, cost);

                pl->refreshInventory();

                mc_utils::sendText(
                    pl,
                    "连锁 {} 个方块, 消耗 {} 点耐久{}, 耗时 {}ms",
                    std::to_string(task.mCount),
                    std::to_string(task.mDeductDamage),
                    Config::cfg.economy.enabled ? ", " + Config::cfg.economy.economyName + std::to_string(cost) : "",
                    duration.count()
                );
            }
            mTaskList.erase(taskID);
        });
    } catch (std::exception& e) {
        mTaskList.erase(taskID);
        logger.error("Fail in running task: {}", e.what());
    }
}