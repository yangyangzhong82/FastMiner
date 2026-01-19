#pragma once
#include "IServerPlayerConfig.h"
#include "config/ConfigBase.h"

namespace fm {
namespace server {

class ServerConfig final : public ConfigBase, public IServerPlayerConfig {
public:
    void load() override;
    void save() override;
    void buildRuntimeConfigMap() override;

    static void _buildDefaultConfig();

    static std::shared_ptr<RuntimeBlockConfig> _buildRuntimeBlockConfig(BlockConfig const& config);

public: /* GUI */
    void addTool(std::string const& blockType, std::string const& toolType);
    void removeTool(std::string const& blockType, std::string const& toolType);

    void addSimilarBlock(std::string const& blockType, std::string const& similarBlockType);
    void removeSimilarBlock(std::string const& blockType, std::string const& similarBlockType);

    void updateBlockConfig(std::string const& oldType, std::string const& newType, BlockConfig config);
    void addBlockConfig(std::string const& blockType, BlockConfig config);
    void removeBlockConfig(std::string const& blockType);

public: /* Player */
    void loadPlayerConfig();
    void savePlayerConfig();

    bool isEnabled(const mce::UUID& uuid, const std::string& key) override;
    void setEnabled(const mce::UUID& uuid, const std::string& key, bool enabled) override;
    void enable(const mce::UUID& uuid, const std::string& key) override;
    void disable(const mce::UUID& uuid, const std::string& key) override;
    bool hasPlayer(const mce::UUID& uuid) override;
    bool hasBlock(const mce::UUID& uuid, const std::string& key) override;
    void removeBlock(const mce::UUID& uuid, const std::string& key) override;
    void ensurePlayerBlockConfig() override;

private:
    PlayerBlockState playerBlockState_;
};

} // namespace server
} // namespace fm
