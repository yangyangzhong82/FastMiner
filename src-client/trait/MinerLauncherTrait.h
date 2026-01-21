#pragma once
#include "Type.h"

#include "../core/ClientMinerLauncher.h"

namespace fm {


template <>
struct internal::ImplType<tag::MinerLauncherTag> {
    using type = client::ClientMinerLauncher;
};


} // namespace fm