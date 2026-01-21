#pragma once
#include "config/ConfigBase.h"

namespace fm::client {

class ClientConfig final : public ConfigBase {
public:
    void load() override;
    void save() override;
    void buildDefaultConfig() override;

    std::shared_ptr<RuntimeBlockConfig> buildRuntimeBlockConfig(BlockConfig const& config) override;

public: /* GUI */
    void addSimilarBlock(std::string const& blockType, std::string const& similarBlockType);
    void removeSimilarBlock(std::string const& blockType, std::string const& similarBlockType);
    void updateBlockConfig(std::string const& oldType, std::string const& newType, BlockConfig config);
    void addBlockConfig(std::string const& blockType, BlockConfig config);
    void removeBlockConfig(std::string const& blockType);
};

} // namespace fm::client