#pragma once
#include "config/ConfigBase.h"

namespace fm::client {

class ClientConfig final : public ConfigBase {
public:
    void load() override;
    void save() override;
    void buildDefaultConfig() override;

    std::shared_ptr<RuntimeBlockConfig> buildRuntimeBlockConfig(BlockConfig const& config) override;
};

} // namespace fm::client