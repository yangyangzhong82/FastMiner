#pragma once
#include "config/ConfigBase.h"

namespace fm::client {

class ClientConfig final : public ConfigBase {
public:
    void load() override;
    void save() override;
    void buildRuntimeConfigMap() override;
};

} // namespace fm::client