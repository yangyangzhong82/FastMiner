#pragma once
#include "Type.h"

#include "../command/ClientFastMinerCommand.h"

namespace fm {

template <>
struct internal::ImplType<tag::CommandTag> {
    using type = client::ClientFastMinerCommand;
};

} // namespace fm