#pragma once
#include "Type.h"

#include "../core/ServerMinerLauncher.h"

namespace fm {

template <>
struct internal::ImplType<tag::MinerLauncherTag> {
    using type = server::ServerMinerLauncher;
};

} // namespace fm