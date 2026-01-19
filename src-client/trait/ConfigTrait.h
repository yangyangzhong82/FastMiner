#pragma once
#include "Type.h"

#include "../config/ClientConfig.h"

namespace fm {


template <>
struct internal::ImplType<tag::ConfigTag> {
    using type = client::ClientConfig;
};


} // namespace fm