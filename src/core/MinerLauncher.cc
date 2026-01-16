#pragma once
#include "core/MinerLauncher.h"
#include "McUtils.h"
#include "config/Config.h"
#include "config/PlayerConfig.h"
#include "core/MinerDispatcher.h"
#include "core/MinerTask.h"
#include "core/MinerTaskContext.h"
#include "core/MinerUtil.h"

#include "ll/api/chrono/GameChrono.h"
#include "ll/api/coro/CoroTask.h"
#include "ll/api/coro/InterruptableSleep.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/ListenerBase.h"
#include "ll/api/event/player/PlayerDestroyBlockEvent.h"
#include "ll/api/event/player/PlayerDisconnectEvent.h"
#include "ll/api/thread/ServerThreadExecutor.h"

#include "mc/platform/UUID.h"
#include "mc/world/item/Item.h"
#include "mc/world/item/ItemStackBase.h"
#include "mc/world/item/enchanting/EnchantUtils.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/Block.h"
#include "mod/FastMiner.h"

#include <atomic>
#include <cstddef>
#include <memory>
#include <utility>

#include "absl/container/flat_hash_set.h"


#ifdef DEBUG
#define FM_TRACE(...) std::cout << __VA_ARGS__ << std::endl
#else
#define FM_TRACE(...) (void)0
#endif

namespace fm {

struct MinerLauncher::Impl {
    std::unique_ptr<MinerDispatcher> dispatcher; // 调度器
    ll::event::ListenerPtr           playerDestroyBlockListener;
    ll::event::ListenerPtr           playerDisconnectListener;
    std::atomic<bool>                abort; // 是否需要中止
    ll::coro::InterruptableSleep     sleep; // 中断等待
};

MinerLauncher::MinerLauncher() : impl(std::make_unique<Impl>()) {
    impl->dispatcher = std::make_unique<MinerDispatcher>();

    auto& bus                        = ll::event::EventBus::getInstance();
    impl->playerDestroyBlockListener = bus.emplaceListener<ll::event::PlayerDestroyBlockEvent>(
        [&](auto& ev) { onPlayerDestroyBlock(ev); },
        ll::event::EventPriority::Low
    );
    impl->playerDisconnectListener = bus.emplaceListener<ll::event::PlayerDisconnectEvent>([&](auto& ev) {
        auto& player = ev.self();
        impl->dispatcher->interruptPlayerTask(player);
    });

    impl->abort = false;
    ll::coro::keepThis([this]() -> ll::coro::CoroTask<> {
        while (!impl->abort) {
            co_await impl->sleep.sleepFor(ll::chrono::ticks{1});
            if (impl->abort) {
                break;
            }
            impl->dispatcher->tick();
        }
        co_return;
    }).launch(ll::thread::ServerThreadExecutor::getDefault());
}

MinerLauncher::~MinerLauncher() {
    impl->abort = true;
    impl->sleep.interrupt(true);
    ll::event::EventBus::getInstance().removeListener(impl->playerDestroyBlockListener);
    ll::event::EventBus::getInstance().removeListener(impl->playerDisconnectListener);
}

void MinerLauncher::onPlayerDestroyBlock(ll::event::PlayerDestroyBlockEvent& ev) {
    auto& rawPos = ev.pos();
    auto& player = ev.self();
    auto  dimId  = player.getDimensionId();

    auto hashedPos = miner_util::hashDimensionPosition(rawPos, dimId);
    if (ev.isCancelled() || impl->dispatcher->isProcessing(hashedPos)) {
        FM_TRACE("event cancelled or processing");
        return; // 已处理 / 正在处理
    }
    if (!impl->dispatcher->canLaunchTask(player)) {
        FM_TRACE("The player has unfinished tasks.");
        return;
    }
    if (!player.isSurvival()) {
        FM_TRACE("player not survival");
        return; // 非生存模式
    }

    auto& blockSource = player.getDimensionBlockSource();
    auto& block       = blockSource.getBlock(rawPos);
    auto& blockType   = block.getTypeName();
    if (!isEnableMiner(player, blockType)) {
        FM_TRACE("player not enable miner");
        return; // 玩家未启用该方块类型 / 未开启连锁
    }
    if (!canDestroyBlockWithMcApi(player, block)) {
        FM_TRACE("player can not destroy block with mc api");
        return; // 玩家无法破坏该方块
    }
    auto rtConfig = Config::getRuntimeBlockConfig(blockType);
    if (!rtConfig) [[unlikely]] {
        FM_TRACE("block type not found in rtConfig");
        return; // 配置文件中没有该方块类型
    }
    if (!canDestroyBlockWithConfig(player, rtConfig)) {
        FM_TRACE("player can not destroy block with config");
        return; // 玩家无法破坏该方块
    }

    MinerTaskContext ctx{
        .player      = player,
        .blockId     = block.getBlockItemId(),
        .tiggerPos   = rawPos,
        .tiggerDimid = dimId,
        .hashedPos   = std::move(hashedPos),
        .blockSource = blockSource,
        .rtConfig    = std::move(rtConfig)
    };
    ll::coro::keepThis([this, ctx = std::move(ctx)]() -> ll::coro::CoroTask<> {
        co_await ll::chrono::ticks{1};
        this->prepareTask(std::move(ctx));
        co_return;
    }).launch(ll::thread::ServerThreadExecutor::getDefault());
}


bool MinerLauncher::isEnableMiner(Player& player, std::string const& blockType) const {
    auto& uuid             = player.getUuid();
    bool  sneakingRequired = PlayerConfig::isEnabled(uuid, PlayerConfig::KEY_SNEAK);
    bool  sneaking         = mc_utils::isSneaking(player);
    return PlayerConfig::isEnabled(uuid, PlayerConfig::KEY_ENABLE) && PlayerConfig::isEnabled(uuid, blockType)
        && (!sneakingRequired || sneaking); // 如果要求下蹲，则必须下蹲；否则忽略
}

void MinerLauncher::prepareTask(MinerTaskContext ctx) {
    auto& block = ctx.blockSource.getBlock(ctx.tiggerPos);
    if (!block.isAir()) {
        FM_TRACE("block is not air");
        return; // 方块不是空气代表着玩家没有破坏它
    }
    auto& itemStack = ctx.player.getSelectedItem();

    int& limit = ctx.limit;
    limit      = ctx.rtConfig->rawConfig_.limit;
    if (!miner_util::hasUnbreakable(itemStack) && itemStack.isDamageableItem()) {
        auto item = itemStack.getItem();
        if (!item) {
            FM_TRACE("item is null");
            return; // 物品为空
        }
        limit = std::min(limit, item->getMaxDamage() - itemStack.getDamageValue() - 1); // 动态计算限制为物品保留1点耐久
#ifdef LL_PLAT_S
        if (Config::cfg.economy.enabled && ctx.rtConfig->rawConfig_.cost > 0) {
            // 动态约束限制为玩家经济
            limit = std::min(
                limit,
                static_cast<int>(
                    FastMiner::getInstance().getEconomy().get(ctx.player.getUuid()) / ctx.rtConfig->rawConfig_.cost
                )
            );
        }
#endif
    }
    if (limit <= 1) {
        FM_TRACE("limit <= 1");
        return; // 限制为1或以下则不进行连锁
    }
    FM_TRACE("limit: " << limit);
    FM_TRACE("Task preparation complete");

    auto task = std::make_shared<MinerTask>(std::move(ctx), *impl->dispatcher);
    impl->dispatcher->launch(task);
}

bool MinerLauncher::canDestroyBlockWithMcApi(Player& player, Block const& block) const {
    // TODO: use BlockSource::checkBlockDestroyPermissions ?
    return player.canDestroyBlock(block) || mc_utils::CanDestroyBlock(player.getSelectedItem(), block)
        || mc_utils::CanDestroySpecial(player.getSelectedItem(), block);
}
bool MinerLauncher::canDestroyBlockWithConfig(Player& player, Config::RuntimeBlockConfig::Ptr const& rtConfig) {
    bool const  hasSilkTouch = EnchantUtils::hasEnchant(Enchant::Type::SilkTouch, player.getSelectedItem());
    auto const& config       = rtConfig->rawConfig_;
    return (config.tools.empty()
            || config.tools.contains(player.getSelectedItem().getTypeName()))           /* 不限制工具 / 工具类型匹配 */
        && (config.silkTouchMode == Config::SilkTouchMode::Unlimited                    /* 不限制精准附魔 */
            || (config.silkTouchMode == Config::SilkTouchMode::Forbid && !hasSilkTouch) /* 禁止精准附魔 */
            || (config.silkTouchMode == Config::SilkTouchMode::Need && hasSilkTouch));  /* 需要精准附魔 */
}

} // namespace fm