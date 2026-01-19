#pragma once
#include "Type.h"

#include <absl/container/flat_hash_set.h>

#include <memory>

#include "trait/ConfigTrait.h"

namespace fm {

// Performance Optimization
struct RuntimeBlockConfig {
    using Ptr         = std::shared_ptr<RuntimeBlockConfig>;
    using BlockConfig = typename internal::ImplType<tag::BlockConfigTag>::type;

    BlockConfig                  rawConfig_;
    absl::flat_hash_set<BlockID> similarBlock_{};
    explicit RuntimeBlockConfig(BlockConfig config) : rawConfig_(std::move(config)) {}
};

} // namespace fm