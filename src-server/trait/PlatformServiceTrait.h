#pragma once
#include "Type.h"

#include "../ServerPlatformService.h"

namespace fm {

template <>
struct internal::ImplType<tag::PlatformServiceTag> {
    using type = server::ServerPlatformService;
};

} // namespace fm