#include "ClientFastMinerCommand.h"

#include <ll/api/command/CommandHandle.h>
#include <mc/server/commands/CommandOutput.h>

namespace fm {
namespace client {


void ClientFastMinerCommand::setup(std::string_view name, std::string_view description) {
    auto& cmd = ll::command::CommandRegistrar::getInstance(true).getOrCreateCommand(name.data(), description.data());

    // fm
    cmd.overload().execute([](CommandOrigin const& origin, CommandOutput& output) {
        if (origin.getOriginType() != CommandOriginType::Player) {
            output.error("Only players can use this command!");
            return;
        }
        // TODO
    });
}


} // namespace client
} // namespace fm