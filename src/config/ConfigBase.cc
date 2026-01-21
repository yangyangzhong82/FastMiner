#include "ConfigBase.h"
#include "RuntimeBlockConfig.h"

#include <concepts>

#include <mc/world/level/block/Block.h>
#include <mc/world/level/block/registry/BlockTypeRegistry.h>

namespace fm {


template <typename T>
concept RequireField = requires {
    requires std::same_as<std::remove_cvref_t<decltype(T::SchemaVersion)>, int>;
    requires std::same_as<std::remove_cvref_t<decltype(std::declval<T>().version)>, int>;
};

static_assert(std::is_aggregate_v<ConfigBase::ConfigModel> && std::is_aggregate_v<RuntimeBlockConfig::BlockConfig>);
static_assert(RequireField<ConfigBase::ConfigModel>);

ConfigBase::~ConfigBase() = default;

void ConfigBase::buildRuntimeConfigMap() {
    runtimeConfigMap.clear();
    for (auto const& [key, value] : data.blocks) {
        auto blockId = getBlockIdCached(key);
        runtimeConfigMap.emplace(blockId, std::move(buildRuntimeBlockConfig(value)));
    }
}

short ConfigBase::getBlockIdCached(std::string const& blockType) {
    auto iter = blockIDCacheMap.find(blockType);
    if (iter == blockIDCacheMap.end()) {
        auto blockId = BlockTypeRegistry::get().getDefaultBlockState(blockType.c_str()).getBlockItemId();

        iter = blockIDCacheMap.emplace(blockType, blockId).first;
    }
    return iter->second;
}
RuntimeBlockConfig::Ptr ConfigBase::getRuntimeBlockConfig(short blockId) {
    auto iter = runtimeConfigMap.find(blockId);
    if (iter == runtimeConfigMap.end()) {
        return nullptr;
    }
    return iter->second;
}
RuntimeBlockConfig::Ptr ConfigBase::getRuntimeBlockConfig(std::string const& blockType) {
    return getRuntimeBlockConfig(getBlockIdCached(blockType));
}


} // namespace fm