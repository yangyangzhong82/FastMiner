#include "ClientPlatformService.h"

#include "Global.h"
#include "command/FastMinerCommand.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/ListenerBase.h"
#include "ll/api/event/client/ClientJoinLevelEvent.h"

namespace fm {
namespace client {

struct ClientPlatformService::Impl {
    ll::event::ListenerPtr mClientJoinLevelListener{nullptr};
};

ClientPlatformService::ClientPlatformService() : impl(std::make_unique<Impl>()) {}

ClientPlatformService::~ClientPlatformService() = default;

bool ClientPlatformService::init() {
    ll::event::EventBus::getInstance().emplaceListener<ll::event::ClientJoinLevelEvent>(
        [](ll::event::ClientJoinLevelEvent&) {
            FM_TRACE("Client joined level, registering commands");
            FastMinerCommand::setup();
        }
    );
    return true;
}

bool ClientPlatformService::destroy() {
    ll::event::EventBus::getInstance().removeListener(impl->mClientJoinLevelListener);
    return true;
}

} // namespace client
} // namespace fm