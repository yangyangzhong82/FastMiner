#pragma once
#include "economy/EconomyConfig.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"

#include <sys/stat.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace fm::Config {


enum class SilkTouchMode : int {
    Unlimited = 0, // 无限制
    Forbid    = 1, // 禁止精准附魔
    Need      = 2  // 需要精准附魔
};

enum class DestroyMode : int {
    Default = 0, // 相邻的6个面
    Cube    = 1  // 3x3x3=27
};

using MinerTools   = std::unordered_set<std::string>; // 工具
using SimilarBlock = std::unordered_set<std::string>; // 类似方块

struct BlockConfig {
    std::string   name;                                    // 名称
    int           cost{0};                                 // 经济
    int           limit{256};                              // 连锁上限
    DestroyMode   destroyMode{DestroyMode::Default};       // 破坏方式
    SilkTouchMode silkTouchMode{SilkTouchMode::Unlimited}; // 精准采集
    MinerTools    tools{};                                 // 连锁采集工具
    SimilarBlock  similarBlock{};                          // 类似方块
};

using Blocks = std::unordered_map<std::string, BlockConfig>;

inline constexpr int ConfigVersion = 6;
struct Impl {
    int version = ConfigVersion;

    struct {
        int globalBlockLimitPerTick{256}; // 全局每tick方块上限
        int maxResumeTasksPerTick{16};    // 每tick最大恢复任务数
    } dispatcher;

    EconomyConfig economy; // 经济系统
    Blocks        blocks;  // 方块配置
};

inline Impl cfg;

// Performance Optimization
struct RuntimeBlockConfig {
    using Ptr = std::shared_ptr<RuntimeBlockConfig>;

    BlockConfig                rawConfig_;
    absl::flat_hash_set<short> similarBlock_{};
    explicit RuntimeBlockConfig(BlockConfig config) : rawConfig_(std::move(config)) {}
};

short                   getBlockIdCached(std::string const& blockType);
RuntimeBlockConfig::Ptr getRuntimeBlockConfig(short blockId);
RuntimeBlockConfig::Ptr getRuntimeBlockConfig(std::string const& blockType);

RuntimeBlockConfig::Ptr _buildRuntimeBlockConfig(BlockConfig const& config);
void                    _buildRuntimeConfigMap();
void                    _buildDefaultConfig();

void load();

void save();

// GUI helper
void dynamicAddTool(std::string const& blockType, std::string const& toolType);
void dynamicRemoveTool(std::string const& blockType, std::string const& toolType);

void dynamicAddSimilarBlock(std::string const& blockType, std::string const& similarBlockType);
void dynamicRemoveSimilarBlock(std::string const& blockType, std::string const& similarBlockType);

void dynamicUpdateBlockConfig(std::string const& oldType, std::string const& newType, BlockConfig config);
void dynamicAddBlockConfig(std::string const& blockType, BlockConfig config);
void dynamicRemoveBlockConfig(std::string const& blockType);

} // namespace fm::Config
