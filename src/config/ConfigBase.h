#pragma once
#include "Type.h"

#include "trait/ConfigTrait.h"

#include <absl/container/flat_hash_map.h>

namespace fm {

struct RuntimeBlockConfig;
struct DispatcherConfig;

struct ConfigBase {
    using BlockIDCacheMap       = absl::flat_hash_map<std::string, BlockID>;
    using RuntimeBlockConfigMap = absl::flat_hash_map<BlockID, std::shared_ptr<RuntimeBlockConfig>>;

    using ConfigModel = typename internal::ImplType<tag::ConfigModelTag>::type;

    inline static ConfigModel           data{};
    inline static RuntimeBlockConfigMap runtimeConfigMap{};

    virtual ~ConfigBase();

    virtual void load() = 0;
    virtual void save() = 0;

    virtual void buildRuntimeConfigMap() = 0;

    inline static DispatcherConfig const& getDispatcherConfig() { return data.dispatcher; }

    static short                               getBlockIdCached(std::string const& blockType);
    static std::shared_ptr<RuntimeBlockConfig> getRuntimeBlockConfig(short blockId);
    static std::shared_ptr<RuntimeBlockConfig> getRuntimeBlockConfig(std::string const& blockType);

    template <std::derived_from<ConfigBase> T>
    inline T& as() {
        return static_cast<T&>(*this);
    }

private:
    inline static BlockIDCacheMap blockIDCacheMap{};
};


} // namespace fm