#pragma once
#include "Type.h"

#include "../ClientPlatformService.h"

namespace fm {

template <>
struct internal::ImplType<tag::PlatformServiceTag> {
    using type = client::ClientPlatformService;
};

} // namespace fm
