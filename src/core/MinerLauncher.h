#pragma once
#include "Global.h"
#include "core/MinerTaskContext.h"

#include "ll/api/event/player/PlayerDestroyBlockEvent.h"

#include <memory>

class Block;
class Player;

namespace fm {


class MinerLauncher final {
    struct Impl;
    std::unique_ptr<Impl> impl;

public:
    FM_DISABLE_COPY_MOVE(MinerLauncher);
    explicit MinerLauncher();
    ~MinerLauncher();

    void onPlayerDestroyBlock(ll::event::PlayerDestroyBlockEvent& ev);

    bool isBlockEnabled(Player& player, std::string const& blockType) const;

    void prepareTask(MinerTaskContext ctx);

    bool        canDestroyBlockWithMcApi(Player& player, Block const& block) const;
    static bool canDestroyBlockWithConfig(Player& player, RuntimeBlockConfig::Ptr const& rtConfig);
};


} // namespace fm