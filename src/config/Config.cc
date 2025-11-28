#include "config/Config.h"
#include "ll/api/Config.h"
#include "mod/FastMiner.h"

#include "mc/deps/core/string/HashedString.h"
#include "mc/world/item/VanillaItemNames.h"
#include "mc/world/level/block/Block.h"
#include "mc/world/level/block/VanillaBlockTypeIds.h"
#include "mc/world/level/block/registry/BlockTypeRegistry.h"

#include <filesystem>
#include <utility>

namespace fm::Config {

using RuntimeConfigMap = absl::flat_hash_map<short, RuntimeBlockConfig::Ptr>;
inline RuntimeConfigMap runtimeConfigMap;

short getBlockIdCached(std::string const& blockType) {
    static absl::flat_hash_map<std::string, short> cache;

    auto iter = cache.find(blockType);
    if (iter == cache.end()) {
        auto blockId = BlockTypeRegistry::get().getDefaultBlockState(blockType.c_str()).getBlockItemId();

        iter = cache.emplace(blockType, blockId).first;
    }
    return iter->second;
}
RuntimeBlockConfig::Ptr getRuntimeBlockConfig(short blockId) {
    auto iter = runtimeConfigMap.find(blockId);
    if (iter == runtimeConfigMap.end()) {
        return nullptr;
    }
    return iter->second;
}
RuntimeBlockConfig::Ptr getRuntimeBlockConfig(std::string const& blockType) {
    return getRuntimeBlockConfig(getBlockIdCached(blockType));
}
RuntimeBlockConfig::Ptr _buildRuntimeBlockConfig(BlockConfig const& config) {
    auto rtConfig = std::make_shared<RuntimeBlockConfig>(config);
    rtConfig->similarBlock_.reserve(config.similarBlock.size());
    for (auto const& similar : config.similarBlock) {
        rtConfig->similarBlock_.insert(getBlockIdCached(similar));
    }
    return rtConfig;
}
void _buildRuntimeConfigMap() {
    runtimeConfigMap.clear();
    for (auto const& [key, value] : cfg.blocks) {
        auto blockId = getBlockIdCached(key);
        runtimeConfigMap.emplace(blockId, std::move(_buildRuntimeBlockConfig(value)));
    }
}
void _buildDefaultConfig() {
    std::unordered_set<std::string> MinecraftAxeTools = {
        VanillaItemNames::WoodenAxe(),
        VanillaItemNames::StoneAxe(),
        VanillaItemNames::IronAxe(),
        VanillaItemNames::DiamondAxe(),
        VanillaItemNames::GoldenAxe(),
        VanillaItemNames::NetheriteAxe()
    };

    cfg.blocks = {
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

void load() {
    _buildDefaultConfig();

    auto path = FastMiner::getInstance().getSelf().getModDir() / "Config.json";
    if (!std::filesystem::exists(path) || !ll::config::loadConfig(cfg, path)) {
        save();
    }

    _buildRuntimeConfigMap();
}

void save() {
    auto path = FastMiner::getInstance().getSelf().getModDir() / "Config.json";
    ll::config::saveConfig(cfg, path);
}

void dynamicAddTool(std::string const& blockType, std::string const& toolType) {
    auto iter = cfg.blocks.find(blockType);
    if (iter != cfg.blocks.end()) {
        iter->second.tools.insert(toolType);
        save();
    }
}
void dynamicRemoveTool(std::string const& blockType, std::string const& toolType) {
    auto iter = cfg.blocks.find(blockType);
    if (iter != cfg.blocks.end()) {
        iter->second.tools.erase(toolType);
        save();
    }
}
void dynamicAddSimilarBlock(std::string const& blockType, std::string const& similarBlockType) {
    auto iter = cfg.blocks.find(blockType);
    if (iter == cfg.blocks.end()) {
        return;
    }
    if (iter->second.similarBlock.insert(similarBlockType).second) {
        save();
        if (auto ptr = getRuntimeBlockConfig(blockType)) {
            ptr->similarBlock_.insert(getBlockIdCached(similarBlockType));
        }
    }
}
void dynamicRemoveSimilarBlock(std::string const& blockType, std::string const& similarBlockType) {
    auto iter = cfg.blocks.find(blockType);
    if (iter == cfg.blocks.end()) {
        return;
    }
    if (iter->second.similarBlock.erase(similarBlockType)) {
        save();
        if (auto ptr = getRuntimeBlockConfig(blockType)) {
            ptr->similarBlock_.erase(getBlockIdCached(similarBlockType));
        }
    }
}
void dynamicUpdateBlockConfig(std::string const& oldType, std::string const& newType, BlockConfig config) {
    if (oldType == newType) {
        auto iter = cfg.blocks.find(oldType);
        if (iter == cfg.blocks.end()) {
            return;
        }
        iter->second = std::move(config);
        save();

        if (auto ptr = getRuntimeBlockConfig(oldType)) {
            ptr->rawConfig_ = iter->second;
        }
        return;
    }

    dynamicRemoveBlockConfig(oldType);
    dynamicAddBlockConfig(newType, std::move(config));
}
void dynamicAddBlockConfig(std::string const& blockType, BlockConfig config) {
    auto result = cfg.blocks.emplace(blockType, std::move(config));
    if (result.second) {
        save();
        if (auto ptr = _buildRuntimeBlockConfig(result.first->second)) {
            runtimeConfigMap.emplace(getBlockIdCached(blockType), ptr);
        }
    }
}
void dynamicRemoveBlockConfig(std::string const& blockType) {
    auto iter = cfg.blocks.find(blockType);
    if (iter == cfg.blocks.end()) {
        return;
    }
    cfg.blocks.erase(iter); // 擦除旧元素
    save();
    runtimeConfigMap.erase(getBlockIdCached(blockType));
}


} // namespace fm::Config