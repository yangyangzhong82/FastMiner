#include "ServerConfig.h"

#include "FastMiner.h"
#include "config/RuntimeBlockConfig.h"
#include "utils/JsonUtils.h"

#include "ll/api/Config.h"

#include "mc/deps/core/string/HashedString.h"
#include "mc/world/item/VanillaItemNames.h"
#include "mc/world/level/block/VanillaBlockTypeIds.h"

#include <filesystem>
#include <utility>

namespace fm {
namespace server {

void ServerConfig::load() {
    auto path = FastMiner::getInstance().getSelf().getModDir() / "Config.json";
    if (!std::filesystem::exists(path) || !ll::config::loadConfig(data, path)) {
        save();
    }
    loadPlayerConfig();
}

void ServerConfig::save() {
    auto path = FastMiner::getInstance().getSelf().getModDir() / "Config.json";
    ll::config::saveConfig(data, path);
    savePlayerConfig();
}

void ServerConfig::buildDefaultConfig() {
    std::unordered_set<std::string> MinecraftAxeTools = {
        VanillaItemNames::WoodenAxe(),
        VanillaItemNames::StoneAxe(),
        VanillaItemNames::IronAxe(),
        VanillaItemNames::DiamondAxe(),
        VanillaItemNames::GoldenAxe(),
        VanillaItemNames::NetheriteAxe()
    };

    data.blocks.clear();
    data.blocks = {
        // clang-format off
        // 树木类
        {VanillaBlockTypeIds::AcaciaLog(), BlockConfig{
            .name = "金合欢木原木",
            .destroyMode = DestroyMode::Cube,
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::BirchLog(), BlockConfig{
            .name = "白桦木原木",
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::CherryLog(), BlockConfig{
            .name = "樱花原木",
            .destroyMode = DestroyMode::Cube,
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::DarkOakLog(), BlockConfig{
            .name = "深色橡木原木",
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::JungleLog(), BlockConfig{
            .name = "丛林原木",
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::MangroveLog(), BlockConfig{
            .name = "红树原木",
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::SpruceLog(), BlockConfig{
            .name = "云杉木原木",
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::OakLog(), BlockConfig{
            .name = "橡木原木",
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::WarpedStem(), BlockConfig{
            .name = "诡异菌柄",
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::CrimsonStem(), BlockConfig{
            .name = "绯红菌柄",
            .tools = MinecraftAxeTools
        }},

        // 矿石类
        {VanillaBlockTypeIds::AncientDebris(), BlockConfig{
            .name = "远古残骸"
        }},
        {VanillaBlockTypeIds::CoalOre(), BlockConfig{
            .name = "煤矿石",
            .similarBlock = {
                VanillaBlockTypeIds::DeepslateCoalOre()
            }
        }},
        {VanillaBlockTypeIds::CopperOre(), BlockConfig{
            .name = "铜矿石",
            .similarBlock = {
                VanillaBlockTypeIds::DeepslateCopperOre()
            }
        }},
        {VanillaBlockTypeIds::DiamondOre(), BlockConfig{
            .name = "钻石矿石",
            .similarBlock = {
                VanillaBlockTypeIds::DeepslateDiamondOre()
            }
        }},
        {VanillaBlockTypeIds::EmeraldOre(), BlockConfig{
            .name = "绿宝石矿石",
            .similarBlock = {
                VanillaBlockTypeIds::DeepslateEmeraldOre()
            }
        }},
        {VanillaBlockTypeIds::GoldOre(), BlockConfig{
            .name = "金矿石",
            .similarBlock = {
                VanillaBlockTypeIds::DeepslateGoldOre()
            }
        }},
        {VanillaBlockTypeIds::IronOre(), BlockConfig{
            .name = "铁矿石",
            .similarBlock = {
                VanillaBlockTypeIds::DeepslateIronOre()
            }
        }},
        {VanillaBlockTypeIds::LapisOre(), BlockConfig{
            .name = "青金石矿石",
            .similarBlock = {
                VanillaBlockTypeIds::DeepslateLapisOre()
            }
        }},
        {VanillaBlockTypeIds::LitRedstoneOre(), BlockConfig{
            .name = "红石矿石",
            .similarBlock = {
                VanillaBlockTypeIds::LitDeepslateRedstoneOre(),
                VanillaBlockTypeIds::RedstoneOre(),
                VanillaBlockTypeIds::DeepslateRedstoneOre()
            }
        }},
        {VanillaBlockTypeIds::NetherGoldOre(), BlockConfig{
            .name = "下界金矿石",
        }},
        {VanillaBlockTypeIds::QuartzOre(), BlockConfig{
            .name = "下界石英矿石"
        }},
        {VanillaBlockTypeIds::DeepslateCoalOre(), BlockConfig{
            .name = "深层煤矿石",
            .similarBlock = {
                VanillaBlockTypeIds::CoalOre()
            }
        }},
        {VanillaBlockTypeIds::DeepslateCopperOre(), BlockConfig{
            .name = "深层铜矿石",
            .similarBlock = {
                VanillaBlockTypeIds::CopperOre()
            }
        }},
        {VanillaBlockTypeIds::DeepslateDiamondOre(), BlockConfig{
            .name = "深层钻石矿石",
            .similarBlock = {
                VanillaBlockTypeIds::DiamondOre()
            }
        }},
        {VanillaBlockTypeIds::DeepslateEmeraldOre(), BlockConfig{
            .name = "深层绿宝石矿石",
            .similarBlock = {
                VanillaBlockTypeIds::EmeraldOre()
            }
        }},
        {VanillaBlockTypeIds::DeepslateGoldOre(), BlockConfig{
            .name = "深层金矿石",
            .similarBlock = {
                VanillaBlockTypeIds::GoldOre()
            }
        }},
        {VanillaBlockTypeIds::DeepslateIronOre(), BlockConfig{
            .name = "深层铁矿石",
            .similarBlock = {
                VanillaBlockTypeIds::IronOre()
            }
        }},
        {VanillaBlockTypeIds::DeepslateLapisOre(), BlockConfig{
            .name = "深层青金石矿石",
            .similarBlock = {
                VanillaBlockTypeIds::LapisOre()
            }
        }},
        {VanillaBlockTypeIds::LitDeepslateRedstoneOre(), BlockConfig{
            .name = "深层红石矿石",
            .similarBlock = {
                VanillaBlockTypeIds::LitRedstoneOre(),
                VanillaBlockTypeIds::RedstoneOre(),
                VanillaBlockTypeIds::DeepslateRedstoneOre()
            }
        }}
        // clang-format on
    };
}
std::shared_ptr<RuntimeBlockConfig> ServerConfig::buildRuntimeBlockConfig(BlockConfig const& config) {
    auto rtConfig   = std::make_shared<RuntimeBlockConfig>(config);
    rtConfig->limit = config.limit;
    rtConfig->similarBlock_.reserve(config.similarBlock.size());
    for (auto const& similar : config.similarBlock) {
        rtConfig->similarBlock_.insert(getBlockIdCached(similar));
    }
    return rtConfig;
}

void ServerConfig::addTool(std::string const& blockType, std::string const& toolType) {
    auto iter = data.blocks.find(blockType);
    if (iter != data.blocks.end()) {
        iter->second.tools.insert(toolType);
        save();
    }
}
void ServerConfig::removeTool(std::string const& blockType, std::string const& toolType) {
    auto iter = data.blocks.find(blockType);
    if (iter != data.blocks.end()) {
        iter->second.tools.erase(toolType);
        save();
    }
}
void ServerConfig::addSimilarBlock(std::string const& blockType, std::string const& similarBlockType) {
    auto iter = data.blocks.find(blockType);
    if (iter == data.blocks.end()) {
        return;
    }
    if (iter->second.similarBlock.insert(similarBlockType).second) {
        save();
        if (auto ptr = getRuntimeBlockConfig(blockType)) {
            ptr->similarBlock_.insert(getBlockIdCached(similarBlockType));
        }
    }
}
void ServerConfig::removeSimilarBlock(std::string const& blockType, std::string const& similarBlockType) {
    auto iter = data.blocks.find(blockType);
    if (iter == data.blocks.end()) {
        return;
    }
    if (iter->second.similarBlock.erase(similarBlockType)) {
        save();
        if (auto ptr = getRuntimeBlockConfig(blockType)) {
            ptr->similarBlock_.erase(getBlockIdCached(similarBlockType));
        }
    }
}
void ServerConfig::updateBlockConfig(std::string const& oldType, std::string const& newType, BlockConfig config) {
    if (oldType == newType) {
        auto iter = data.blocks.find(oldType);
        if (iter == data.blocks.end()) {
            return;
        }
        iter->second = std::move(config);
        save();

        if (auto ptr = getRuntimeBlockConfig(oldType)) {
            ptr->rawConfig_ = iter->second;
        }
        return;
    }

    removeBlockConfig(oldType);
    addBlockConfig(newType, std::move(config));
}
void ServerConfig::addBlockConfig(std::string const& blockType, BlockConfig config) {
    auto result = data.blocks.emplace(blockType, std::move(config));
    if (result.second) {
        save();
        if (auto ptr = buildRuntimeBlockConfig(result.first->second)) {
            runtimeConfigMap.emplace(getBlockIdCached(blockType), ptr);
        }
    }
}
void ServerConfig::removeBlockConfig(std::string const& blockType) {
    auto iter = data.blocks.find(blockType);
    if (iter == data.blocks.end()) {
        return;
    }
    data.blocks.erase(iter); // 擦除旧元素
    save();
    runtimeConfigMap.erase(getBlockIdCached(blockType));
}

void ServerConfig::loadPlayerConfig() {
    auto& mod  = FastMiner::getInstance().getSelf();
    auto  path = mod.getModDir() / PlayerConfigFileName;

    if (!std::filesystem::exists(path)) {
        save();
        return;
    }

    auto content = ll::file_utils::readFile(path);
    if (!content) {
        throw std::runtime_error("Failed to read player config file");
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(*content);
    } catch (nlohmann::json::parse_error const& e) {
        throw std::runtime_error("Failed to parse player config file: " + std::string(e.what()));
    }

    json_utils::json2struct(playerBlockState_, json);
    ensurePlayerBlockConfig();
}
void ServerConfig::savePlayerConfig() {
    auto& mod  = FastMiner::getInstance().getSelf();
    auto  path = mod.getModDir() / PlayerConfigFileName;

    auto json = json_utils::struct2json(playerBlockState_);
    ll::file_utils::writeFile(path, json.dump());
}


bool ServerConfig::isEnabled(const mce::UUID& uuid, const std::string& key) {
    auto iter = playerBlockState_.find(uuid);
    if (iter == playerBlockState_.end()) {
        return false;
    }
    return iter->second[key];
}
void ServerConfig::setEnabled(const mce::UUID& uuid, const std::string& key, bool enabled) {
    auto iter = playerBlockState_.find(uuid);
    if (iter == playerBlockState_.end()) {
        playerBlockState_[uuid] = {
            {key, enabled}
        };
        return;
    }
    iter->second[key] = enabled;
}
void ServerConfig::enable(const mce::UUID& uuid, const std::string& key) { setEnabled(uuid, key, true); }
void ServerConfig::disable(const mce::UUID& uuid, const std::string& key) { setEnabled(uuid, key, false); }
bool ServerConfig::hasPlayer(const mce::UUID& uuid) { return playerBlockState_.find(uuid) != playerBlockState_.end(); }
bool ServerConfig::hasBlock(const mce::UUID& uuid, const std::string& key) {
    auto iter = playerBlockState_.find(uuid);
    if (iter == playerBlockState_.end()) {
        return false;
    }
    return iter->second.find(key) != iter->second.end();
}
void ServerConfig::removeBlock(const mce::UUID& uuid, const std::string& key) {
    auto iter = playerBlockState_.find(uuid);
    if (iter == playerBlockState_.end()) {
        return;
    }
    iter->second.erase(key);
}
void ServerConfig::ensurePlayerBlockConfig() {
    auto const& blocks = data.blocks;
    for (auto&& [uuid, bls] : playerBlockState_) {

        auto iter = bls.begin();
        while (iter != bls.end()) {
            auto const& key = iter->first;
            if (blocks.find(key) == blocks.end() && key != KEY_ENABLE && key != KEY_SNEAK) {
                iter = bls.erase(iter);
            } else {
                ++iter;
            }
        }
    }
    savePlayerConfig();
}


} // namespace server
} // namespace fm