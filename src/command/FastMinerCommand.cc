#include "FastMinerCommand.h"
#include "McUtils.h"
#include "config/Config.h"
#include "config/PlayerConfig.h"
#include "gui/Form.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/FormBase.h"
#include "ll/api/form/SimpleForm.h"
#include "magic_enum.hpp"
#include "mc/nbt/ByteTag.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/item/Item.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/item/SaveContext.h"
#include "mc/world/item/SaveContextFactory.h"
#include "mc/world/level/block/Block.h"
#include "mod/FastMiner.h"
#include <cstdint>
#include <fmt/core.h>
#include <initializer_list>
#include <ll/api/command/Command.h>
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/command/Optional.h>
#include <ll/api/command/Overload.h>
#include <ll/api/i18n/I18n.h>
#include <ll/api/io/Logger.h>
#include <ll/api/service/Bedrock.h>
#include <ll/api/service/PlayerInfo.h>
#include <ll/api/service/Service.h>
#include <ll/api/utils/HashUtils.h>
#include <mc/network/packet/LevelChunkPacket.h>
#include <mc/network/packet/TextPacket.h>
#include <mc/server/ServerLevel.h>
#include <mc/server/ServerPlayer.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOriginType.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandParameterOption.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/server/commands/CommandRegistry.h>
#include <mc/server/commands/CommandSelector.h>
#include <mc/world/actor/Actor.h>
#include <mc/world/actor/player/Player.h>
#include <sstream>


namespace fm {


struct StatusOption {
    enum class Status : bool { off = false, on = true } state;
};

inline constexpr auto ERR_ONLY_PLAYER_USE = "This command can only be used by players.";

void FastMinerCommand::setup() {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand("fm", "FastMiner - 连锁采集");

    // fm
    cmd.overload().execute([](CommandOrigin const& ori, CommandOutput& out) {
        if (ori.getOriginType() != CommandOriginType::Player) {
            return mc_utils::sendText<mc_utils::LogLevel::Error>(out, ERR_ONLY_PLAYER_USE);
        }
        Player& pl = *static_cast<Player*>(ori.getEntity());
        gui::sendPlayerConfigGUI(pl);
    });

    // fm <on/off>
    cmd.overload<StatusOption>().required("state").execute(
        [](CommandOrigin const& ori, CommandOutput& out, StatusOption const& opt) {
            if (ori.getOriginType() != CommandOriginType::Player) {
                return mc_utils::sendText<mc_utils::LogLevel::Error>(out, ERR_ONLY_PLAYER_USE);
            }
            Player& pl = *static_cast<Player*>(ori.getEntity());

            PlayerConfig::setEnabled(pl.getUuid().asString(), PlayerConfig::KEY_ENABLE, (bool)opt.state);
            mc_utils::sendText(pl, "设置已保存");
        }
    );

    // fm manager
    cmd.overload().text("manager").execute([](CommandOrigin const& ori, CommandOutput& out) {
        if (ori.getOriginType() != CommandOriginType::Player) {
            return mc_utils::sendText<mc_utils::LogLevel::Error>(out, ERR_ONLY_PLAYER_USE);
        }
        Player& pl = *static_cast<Player*>(ori.getEntity());
        if (!pl.isOperator()) {
            return mc_utils::sendText<mc_utils::LogLevel::Error>(out, "无权限");
        }
        gui::sendOpBlockManager(pl);
    });

#ifdef DEBUG
    // fm debug unbreakable
    cmd.overload().text("debug").text("unbreakable").execute([](CommandOrigin const& ori, CommandOutput& out) {
        if (ori.getOriginType() != CommandOriginType::Player) {
            return mc_utils::sendText<mc_utils::LogLevel::Error>(out, ERR_ONLY_PLAYER_USE);
        }
        Player& pl = *static_cast<Player*>(ori.getEntity());

        auto& item = const_cast<ItemStack&>(pl.getSelectedItem());
        if (item.isNull()) {
            return mc_utils::sendText<mc_utils::LogLevel::Error>(out, "请先选择一个物品");
        }

        auto nbt = item.save(*SaveContextFactory::createCloneSaveContext());

        CompoundTagVariant* tag;
        if (nbt->contains("tag")) {
            tag = &(*nbt)["tag"];
        } else {
            CompoundTagVariant t{};
            tag = &t;
        }
        (*tag)["Unbreakable"] = ByteTag{true};

        item._loadItem(*nbt);
        auto mItem = item.mItem;
        if (mItem) {
            mItem->fixupCommon(item);
            if (item.getAuxValue() == 0x7FFF) {
                item.mAuxValue = 0;
            }
        }

        item.setCustomLore({"Unbreakable"});
        pl.refreshInventory();

        mc_utils::sendText(pl, "设置已保存");
    });
#endif
}


} // namespace fm