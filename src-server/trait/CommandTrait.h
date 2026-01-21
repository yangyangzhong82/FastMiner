#pragma once
#include "Type.h"

#include "../command/ServerFastMinerCommand.h"

namespace fm {

template <>
struct internal::ImplType<tag::CommandTag> {
    using type = server::ServerFastMinerCommand;
};

} // namespace fm