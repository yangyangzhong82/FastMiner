#pragma once
#include "core/MinerLauncher.h"
#include "McUtils.h"
#include "config/Config.h"
#include "config/PlayerConfig.h"
#include "core/MinerDispatcher.h"
#include "core/MinerTask.h"
#include "core/MinerTaskContext.h"
#include "core/MinerUtil.h"
#include "economy/EconomySystem.h"

#include "ll/api/chrono/GameChrono.h"
#include "ll/api/coro/CoroTask.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/ListenerBase.h"
#include "ll/api/event/player/PlayerDestroyBlockEvent.h"
#include "ll/api/thread/ServerThreadExecutor.h"

#include "mc/world/item/Item.h"
#include "mc/world/item/ItemStackBase.h"
#include "mc/world/item/enchanting/EnchantUtils.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/Block.h"

#include <memory>
#include <utility>


#ifdef DEBUG
#define FM_TRACE(...) std::cout << __VA_ARGS__ << std::endl
#else
#define FM_TRACE(...) (void)0
#endif

namespace fm {


MinerLauncher::MinerLauncher() {
    dispatcher                 = std::make_unique<MinerDispatcher>();
    playerDestroyBlockListener = ll::event::EventBus::getInstance().emplaceListener<ll::event::PlayerDestroyBlockEvent>(
        [&](auto& ev) { onPlayerDestroyBlock(ev); },
        ll::event::EventPriority::Low
    );
    abort = false;
    ll::coro::keepThis([this]() -> ll::coro::CoroTask<> {
        while (!abort) {
            co_await sleep.sleepFor(ll::chrono::ticks{1});
            if (abort) {
                break;
            }
            dispatcher->tick();
        }
        co_return;
    }).launch(ll::thread::ServerThreadExecutor::getDefault());
}

MinerLauncher::~MinerLauncher() {
    abort = true;
    sleep.interrupt(true);
    ll::event::EventBus::getInstance().removeListener(playerDestroyBlockListener);
}

void MinerLauncher::onPlayerDestroyBlock(ll::event::PlayerDestroyBlockEvent& ev) {
    auto& rawPos = ev.pos();
    auto& player = ev.self();
    auto  dimId  = player.getDimensionId();

    auto hashedPos = MinerUtil::hashDimensionPosition(rawPos, dimId);
    if (ev.isCancelled() || dispatcher->isProcessing(hashedPos)) {
        FM_TRACE("event cancelled or processing");
        return; // 已处理 / 正在处理
    }
    if (!dispatcher->canLaunchTask(player)) {
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
    auto iter = Config::cfg.blocks.find(blockType);
    if (iter == Config::cfg.blocks.end()) [[unlikely]] {
        FM_TRACE("block type not found in config");
        return; // 配置文件中没有该方块类型
    }
    auto const& blockConfig = iter->second;
    if (!canDestroyBlockWithConfig(player, blockConfig)) {
        FM_TRACE("player can not destroy block with config");
        return; // 玩家无法破坏该方块
    }

    MinerTaskContext ctx{
        .player      = player,
        .tiggerPos   = rawPos,
        .tiggerDimid = dimId,
        .hashedPos   = std::move(hashedPos),
        .block       = block,
        .blockSource = blockSource,
        .blockConfig = blockConfig
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
    if (!ctx.block.isAir()) {
        FM_TRACE("block is not air");
        return; // 方块不是空气代表着玩家没有破坏它
    }
    auto& itemStack = ctx.player.getSelectedItem();

    int& limit = ctx.limit;
    limit      = ctx.blockConfig.limit;
    if (!MinerUtil::hasUnbreakable(itemStack) && itemStack.isDamageableItem()) {
        auto item = itemStack.getItem();
        if (!item) {
            FM_TRACE("item is null");
            return; // 物品为空
        }
        limit = std::min(limit, item->getMaxDamage() - itemStack.getDamageValue() - 1); // 动态计算限制为物品保留1点耐久
        if (Config::cfg.economy.enabled && ctx.blockConfig.cost > 0) {
            // 动态约束限制为玩家经济
            limit =
                std::min(limit, static_cast<int>(EconomySystem::getInstance()->get(ctx.player) / ctx.blockConfig.cost));
        }
    }
    if (limit <= 1) {
        FM_TRACE("limit <= 1");
        return; // 限制为1或以下则不进行连锁
    }

    FM_TRACE("Task preparation complete");

    auto task = std::make_shared<MinerTask>(std::move(ctx), *dispatcher);
    dispatcher->launch(task);
}

bool MinerLauncher::canDestroyBlockWithMcApi(Player& player, Block const& block) const {
    return player.canDestroyBlock(block) || mc_utils::CanDestroyBlock(player.getSelectedItem(), block)
        || mc_utils::CanDestroySpecial(player.getSelectedItem(), block);
}
bool MinerLauncher::canDestroyBlockWithConfig(Player& player, Config::BlockConfig const& config) const {
    bool const hasSilkTouch = EnchantUtils::hasEnchant(Enchant::Type::SilkTouch, player.getSelectedItem());
    return (config.tools.empty()
            || config.tools.contains(player.getSelectedItem().getTypeName()))           /* 不限制工具 / 工具类型匹配 */
        && (config.silkTouchMode == Config::SilkTouchMode::Unlimited                    /* 不限制精准附魔 */
            || (config.silkTouchMode == Config::SilkTouchMode::Forbid && !hasSilkTouch) /* 禁止精准附魔 */
            || (config.silkTouchMode == Config::SilkTouchMode::Need && hasSilkTouch));  /* 需要精准附魔 */
}

} // namespace fm