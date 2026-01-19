#pragma once
#include "core/DispatcherConfig.h"

#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace fm::client {

struct BlockConfig {
    std::string                     name;
    std::optional<int>              limit{std::nullopt};
    DestroyMode                     destroyMode{DestroyMode::Default};
    std::unordered_set<std::string> similarBlock{};
};

using Blocks = std::unordered_map<std::string, BlockConfig>;

struct ClientConfigModel {
    static int constexpr SchemaVersion = 1;

    int version = SchemaVersion;

    DispatcherConfig dispatcher;

    int bindKey{86}; // Windows VK Codes

    Blocks blocks;
};

} // namespace fm::client