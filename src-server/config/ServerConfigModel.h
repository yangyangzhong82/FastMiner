#pragma once
#include "core/DispatcherConfig.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace fm::server {

enum class SilkTouchMode : int {
    Unlimited = 0, // 无限制
    Forbid    = 1, // 禁止精准附魔
    Need      = 2  // 需要精准附魔
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

struct ServerConfigModel {
    static constexpr int SchemaVersion = 7;

    int version = SchemaVersion;

    DispatcherConfig dispatcher;

    struct EconomyConfig {
        enum class EconomyKit { LegacyMoney, ScoreBoard };

        bool        enabled        = false;
        EconomyKit  kit            = EconomyKit::LegacyMoney;
        std::string scoreboardName = "Scoreboard";
    } economy;

    Blocks blocks; // 方块配置
};

} // namespace fm::server
