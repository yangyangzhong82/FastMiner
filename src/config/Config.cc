#include "config/Config.h"
#include "ll/api/Config.h"
#include "mod/FastMiner.h"
#include <filesystem>

#include "mc/deps/core/string/HashedString.h"

#include "mc/world/item/VanillaItemNames.h"

#include "mc/world/level/block/VanillaBlockTypeIds.h"

namespace fm::Config {

bool buildDefaultConfig() {
    MinecraftAxeTools = {
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
            .destroyMod = DestroyMode::Cube,
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::BirchLog(), {
            .name = "白桦木原木",
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::CherryLog(), {
            .name = "樱花原木",
            .destroyMod = DestroyMode::Cube,
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::DarkOakLog(), {
            .name = "深色橡木原木",
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::JungleLog(), {
            .name = "丛林原木",
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::MangroveLog(), {
            .name = "红树原木",
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::SpruceLog(), {
            .name = "云杉木原木",
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::OakLog(), {
            .name = "橡木原木",
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::WarpedStem(), {
            .name = "诡异菌柄",
            .tools = MinecraftAxeTools
        }},
        {VanillaBlockTypeIds::CrimsonStem(), {
            .name = "绯红菌柄",
            .tools = MinecraftAxeTools
        }},

        // 矿石类
        {VanillaBlockTypeIds::AncientDebris(), {
            .name = "远古残骸"
        }},
        {VanillaBlockTypeIds::CoalOre(), {
            .name = "煤矿石",
            .similarBlock = {
                VanillaBlockTypeIds::DeepslateCoalOre()
            }
        }},
        {VanillaBlockTypeIds::CopperOre(), {
            .name = "铜矿石",
            .similarBlock = {
                VanillaBlockTypeIds::DeepslateCopperOre()
            }
        }},
        {VanillaBlockTypeIds::DiamondOre(), {
            .name = "钻石矿石",
            .similarBlock = {
                VanillaBlockTypeIds::DeepslateDiamondOre()
            }
        }},
        {VanillaBlockTypeIds::EmeraldOre(), {
            .name = "绿宝石矿石",
            .similarBlock = {
                VanillaBlockTypeIds::DeepslateEmeraldOre()
            }
        }},
        {VanillaBlockTypeIds::GoldOre(), {
            .name = "金矿石",
            .similarBlock = {
                VanillaBlockTypeIds::DeepslateGoldOre()
            }
        }},
        {VanillaBlockTypeIds::IronOre(), {
            .name = "铁矿石",
            .similarBlock = {
                VanillaBlockTypeIds::DeepslateIronOre()
            }
        }},
        {VanillaBlockTypeIds::LapisOre(), {
            .name = "青金石矿石",
            .similarBlock = {
                VanillaBlockTypeIds::DeepslateLapisOre()
            }
        }},
        {VanillaBlockTypeIds::LitRedstoneOre(), {
            .name = "红石矿石",
            .similarBlock = {
                VanillaBlockTypeIds::LitDeepslateRedstoneOre(),
                VanillaBlockTypeIds::RedstoneOre(),
                VanillaBlockTypeIds::DeepslateRedstoneOre()
            }
        }},
        {VanillaBlockTypeIds::NetherGoldOre(), {
            .name = "下界金矿石",
        }},
        {VanillaBlockTypeIds::QuartzOre(), {
            .name = "下界石英矿石"
        }},
        {VanillaBlockTypeIds::DeepslateCoalOre(), {
            .name = "深层煤矿石",
            .similarBlock = {
                VanillaBlockTypeIds::CoalOre()
            }
        }},
        {VanillaBlockTypeIds::DeepslateCopperOre(), {
            .name = "深层铜矿石",
            .similarBlock = {
                VanillaBlockTypeIds::CopperOre()
            }
        }},
        {VanillaBlockTypeIds::DeepslateDiamondOre(), {
            .name = "深层钻石矿石",
            .similarBlock = {
                VanillaBlockTypeIds::DiamondOre()
            }
        }},
        {VanillaBlockTypeIds::DeepslateEmeraldOre(), {
            .name = "深层绿宝石矿石",
            .similarBlock = {
                VanillaBlockTypeIds::EmeraldOre()
            }
        }},
        {VanillaBlockTypeIds::DeepslateGoldOre(), {
            .name = "深层金矿石",
            .similarBlock = {
                VanillaBlockTypeIds::GoldOre()
            }
        }},
        {VanillaBlockTypeIds::DeepslateIronOre(), {
            .name = "深层铁矿石",
            .similarBlock = {
                VanillaBlockTypeIds::IronOre()
            }
        }},
        {VanillaBlockTypeIds::DeepslateLapisOre(), {
            .name = "深层青金石矿石",
            .similarBlock = {
                VanillaBlockTypeIds::LapisOre()
            }
        }},
        {VanillaBlockTypeIds::LitDeepslateRedstoneOre(), {
            .name = "深层红石矿石",
            .similarBlock = {
                VanillaBlockTypeIds::LitRedstoneOre(),
                VanillaBlockTypeIds::RedstoneOre(),
                VanillaBlockTypeIds::DeepslateRedstoneOre()
            }
        }},
        // clang-format on
    };
    return true;
}

void load() {
    auto path = FastMiner::getInstance().getSelf().getModDir() / "Config.json";

    if (!std::filesystem::exists(path)) {
        save();
        return;
    }

    bool ok = ll::config::loadConfig(cfg, path);
    if (!ok) {
        save();
    }
}

void save() {
    auto path = FastMiner::getInstance().getSelf().getModDir() / "Config.json";

    ll::config::saveConfig(cfg, path);
}


} // namespace fm::Config