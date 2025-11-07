#pragma once
#include "economy/EconomyConfig.h"
#include <sys/stat.h>
#include <unordered_map>
#include <unordered_set>


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

inline std::unordered_set<std::string> MinecraftAxeTools;

bool buildDefaultConfig();

void load();

void save();


} // namespace fm::Config
