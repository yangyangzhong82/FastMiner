#pragma once
#include "Type.h"

#include "../config/ClientConfig.h"

namespace fm {

template <>
struct internal::ImplType<tag::ConfigFactoryTag> {
    struct Factory {
        std::unique_ptr<client::ClientConfig> operator()() { return std::make_unique<client::ClientConfig>(); }
    };
    using type = Factory;
};

} // namespace fm