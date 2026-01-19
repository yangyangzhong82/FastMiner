#pragma once
#include "Type.h"

#include "../config/ServerConfig.h"

#include <memory>

namespace fm {

template <>
struct internal::ImplType<tag::ConfigFactoryTag> {
    struct Factory {
        std::unique_ptr<server::ServerConfig> operator()() { return std::make_unique<server::ServerConfig>(); }
    };

    using type = Factory;
};

} // namespace fm