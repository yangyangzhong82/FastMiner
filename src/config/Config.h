#pragma once
#include "EconomySystem.h"
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

struct BlockItem {
    std::string   name;                                   // 名称
    int           cost{0};                                // 经济
    int           limit{256};                             // 连锁上限
    DestroyMode   destroyMod{DestroyMode::Default};       // 破坏方式
    SilkTouchMode silkTouchMod{SilkTouchMode::Unlimited}; // 精准采集
    MinerTools    tools{};                                // 连锁采集工具
    SimilarBlock  similarBlock{};                         // 类似方块
};

using Blocks = std::unordered_map<std::string, BlockItem>;


#define MINECRAFT_AXE_TOOL                                                                                             \
    {"minecraft:wooden_axe",                                                                                           \
     "minecraft:stone_axe",                                                                                            \
     "minecraft:iron_axe",                                                                                             \
     "minecraft:diamond_axe",                                                                                          \
     "minecraft:golden_axe",                                                                                           \
     "minecraft:netherite_axe"}


struct Impl {
    int version = 3;

    EconomySystemConfig economy; // 经济系统

    Blocks blocks = {
        // clang-format off
        {"minecraft:acacia_log", {
            .name = "金合欢木原木",
            .destroyMod = DestroyMode::Cube,
            .tools = MINECRAFT_AXE_TOOL
        }},
        {"minecraft:ancient_debris", {
            .name = "远古残骸"
        }},
        {"minecraft:birch_log", {
            .name = "白桦木原木",
            .tools = MINECRAFT_AXE_TOOL
        }},
        {"minecraft:cherry_log", {
            .name = "樱花原木",
            .destroyMod = DestroyMode::Cube,
            .tools = MINECRAFT_AXE_TOOL
        }},
        {"minecraft:coal_ore", {
            .name = "煤矿石",
            .similarBlock = {
                "minecraft:deepslate_coal_ore"
            }
        }},
        {"minecraft:copper_ore", {
            .name = "铜矿石",
            .similarBlock = {
                "minecraft:deepslate_copper_ore"
            }
        }},
        {"minecraft:crimson_stem", {
            .name = "绯红菌柄",
            .tools = MINECRAFT_AXE_TOOL
        }},
        {"minecraft:dark_oak_log", {
            .name = "深色橡木原木",
            .tools = MINECRAFT_AXE_TOOL
        }},
        {"minecraft:deepslate_coal_ore", {
            .name = "深层煤矿石",
            .similarBlock = {
                "minecraft:coal_ore"
            }
        }},
        {"minecraft:deepslate_copper_ore", {
            .name = "深层铜矿石",
            .similarBlock = {
                "minecraft:copper_ore"
            }
        }},
        {"minecraft:deepslate_diamond_ore", {
            .name = "深层钻石矿石",
            .similarBlock = {
                "minecraft:diamond_ore"
            }
        }},
        {"minecraft:deepslate_emerald_ore", {
            .name = "深层绿宝石矿石",
            .similarBlock = {
                "minecraft:emerald_ore"
            }
        }},
        {"minecraft:deepslate_gold_ore", {
            .name = "深层金矿石",
            .similarBlock = {
                "minecraft:gold_ore"
            }
        }},
        {"minecraft:deepslate_iron_ore", {
            .name = "深层铁矿石",
            .similarBlock = {
                "minecraft:iron_ore"
            }
        }},
        {"minecraft:deepslate_lapis_ore", {
            .name = "深层青金石矿石",
            .similarBlock = {
                "minecraft:lapis_ore"
            }
        }},
        {"minecraft:diamond_ore", {
            .name = "钻石矿石",
            .similarBlock = {
                "minecraft:deepslate_diamond_ore"
            }
        }},
        {"minecraft:emerald_ore", {
            .name = "绿宝石矿石",
            .similarBlock = {
                "minecraft:deepslate_emerald_ore"
            }
        }},
        {"minecraft:gold_ore", {
            .name = "金矿石",
            .similarBlock = {
                "minecraft:deepslate_gold_ore"
            }
        }},
        {"minecraft:iron_ore", {
            .name = "铁矿石",
            .similarBlock = {
                "minecraft:deepslate_iron_ore"
            }
        }},
        {"minecraft:jungle_log", {
            .name = "丛林原木",
            .tools = MINECRAFT_AXE_TOOL
        }},
        {"minecraft:lapis_ore", {
            .name = "青金石矿石",
            .similarBlock = {
                "minecraft:deepslate_lapis_ore"
            }
        }},
        {"minecraft:lit_deepslate_redstone_ore", {
            .name = "深层红石矿石",
            .similarBlock = {
                "minecraft:lit_redstone_ore",
                "minecraft:redstone_ore",
                "minecraft:deepslate_redstone_ore"
            }
        }},
        {"minecraft:lit_redstone_ore", {
            .name = "红石矿石",
            .similarBlock = {
                "minecraft:lit_deepslate_redstone_ore",
                "minecraft:redstone_ore",
                "minecraft:deepslate_redstone_ore"
            }
        }},
        {"minecraft:mangrove_log", {
            .name = "红树原木",
            .tools = MINECRAFT_AXE_TOOL
        }},
        {"minecraft:nether_gold_ore", {
            .name = "下界金矿石",
        }},
        {"minecraft:oak_log", {
            .name = "橡木原木",
            .tools = MINECRAFT_AXE_TOOL
        }},
        {"minecraft:quartz_ore", {
            .name = "下界石英矿石"
        }},
        {"minecraft:spruce_log", {
            .name = "云杉木原木",
            .tools = MINECRAFT_AXE_TOOL
        }},
        {"minecraft:warped_stem", {
            .name = "诡异菌柄",
            .tools = MINECRAFT_AXE_TOOL
        }}
        // clang-format on
    };
};

inline Impl cfg;

void load();

void save();


} // namespace fm::Config