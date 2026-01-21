#include "ClientFastMinerCommand.h"

#include "gui/ClientGUI.h"

#include <ll/api/command/CommandHandle.h>

#include "mc/world/actor/Actor.h"
#include "mc/world/actor/player/Player.h"
#include <mc/server/commands/CommandOutput.h>

namespace fm {
namespace client {


void ClientFastMinerCommand::setup(std::string_view name, std::string_view description) {
    // TODO: 由于加载器问题，这里暂时注册为服务端命令(false)，待修复后更改回客户端命令(true)
    // https://github.com/LiteLDev/LeviLamina/issues/1778
    auto& cmd = ll::command::CommandRegistrar::getInstance(false).getOrCreateCommand(name.data(), description.data());

    // fm
    cmd.overload().execute([](CommandOrigin const& origin, CommandOutput& output) {
        if (origin.getOriginType() != CommandOriginType::Player) {
            output.error("Only players can use this command!");
            return;
        }
        auto& player = *static_cast<Player*>(origin.getEntity());
        ClientGUI::sendTo(player);
    });
}


} // namespace client
} // namespace fm