#pragma once
#include "Global.h"
#include "config/Config.h"
#include "core/MinerDispatcher.h"
#include "core/MinerTaskContext.h"
#include "core/MinerUtil.h"

#include "ll/api/coro/InterruptableSleep.h"
#include "ll/api/event/ListenerBase.h"
#include "ll/api/event/player/PlayerDestroyBlockEvent.h"


#include "absl/container/flat_hash_set.h"
#include "mc/platform/UUID.h"

#include <atomic>
#include <cstddef>
#include <memory>

class Block;
class Player;

namespace fm {


class MinerLauncher final {
    std::unique_ptr<MinerDispatcher> dispatcher; // 调度器
    ll::event::ListenerPtr           playerDestroyBlockListener;
    ll::event::ListenerPtr           playerDisconnectListener;
    std::atomic<bool>                abort; // 是否需要中止
    ll::coro::InterruptableSleep     sleep; // 中断等待

public:
    FM_DISABLE_COPY_MOVE(MinerLauncher);
    explicit MinerLauncher();
    ~MinerLauncher();

    void onPlayerDestroyBlock(ll::event::PlayerDestroyBlockEvent& ev);

    bool isEnableMiner(Player& player, std::string const& blockType) const;

    void prepareTask(MinerTaskContext ctx);

    bool canDestroyBlockWithMcApi(Player& player, Block const& block) const;
    static bool canDestroyBlockWithConfig(Player& player, Config::RuntimeBlockConfig::Ptr const& rtConfig);
};


} // namespace fm