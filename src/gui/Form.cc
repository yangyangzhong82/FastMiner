#include "gui/Form.h"
#include "Config/Config.h"
#include "McUtils.h"
#include "config/Config.h"
#include "config/PlayerConfig.h"
#include "fmt/format.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/FormBase.h"
#include "ll/api/form/SimpleForm.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/level/block/Block.h"
#include "utils/JsonUtils.h"
#include <mc/world/actor/Actor.h>
#include <mc/world/actor/player/Player.h>
#include <string>
#include <vector>


using namespace ll::form;

namespace fm::gui {

inline std::vector<std::string> const DestroyModeType = {"默认(相邻的6个方块)", "立方体(3x3x3)"};
inline std::unordered_map<std::string, Config::DestroyMode> const DestroyModeMap = {
    {DestroyModeType[0], Config::DestroyMode::Default},
    {DestroyModeType[1], Config::DestroyMode::Cube   }
};

inline std::vector<std::string> const SilkTouchType = {"无限制", "禁用精准采集", "仅限精准采集"};
inline std::unordered_map<std::string, Config::SilkTouchMode> const SilkTouchMap = {
    {SilkTouchType[0], Config::SilkTouchMode::Unlimited},
    {SilkTouchType[1], Config::SilkTouchMode::Forbid   },
    {SilkTouchType[2], Config::SilkTouchMode::Need     }
};

void __sendEditBlockTools(Player& player, std::string const& typeName) {
    auto& tools = Config::cfg.blocks[typeName].tools;

    SimpleForm f{PLUGIN_NAME};
    f.appendButton("返回", "textures/ui/icon_import", "path", [typeName](Player& pl) {
        _sendBlockViewer(pl, typeName);
    });
    f.appendButton("添加手持工具", "textures/ui/color_plus", "path", [typeName](Player& pl) {
        auto const& item = pl.getSelectedItem();
        if (item.isNull() || item.isBlock()) {
            mc_utils::sendText<mc_utils::LogLevel::Error>(pl, "请手持一个工具!");
            return;
        }
        Config::dynamicAddTool(typeName, item.getTypeName());
        __sendEditBlockTools(pl, typeName);
    });
    f.appendDivider();
    for (auto const& tool : tools) {
        ItemStack item{tool};
        f.appendButton(
            fmt::format("{}\n点击移除工具", item.isNull() ? tool : item.getName()),
            [tool, typeName]([[maybe_unused]] Player& pl) {
                Config::dynamicRemoveTool(typeName, tool);
                __sendEditBlockTools(pl, typeName);
            }
        );
    }
    f.sendTo(player);
}

void __sendEditSimilarBlock(Player& player, std::string const& typeName) {
    auto& similarBlock = Config::cfg.blocks[typeName].similarBlock;

    SimpleForm f{PLUGIN_NAME};
    f.appendButton("返回", "textures/ui/icon_import", "path", [typeName](Player& pl) {
        _sendBlockViewer(pl, typeName);
    });
    f.appendButton("添加手持方块", "textures/ui/color_plus", "path", [typeName](Player& pl) {
        auto const& item = pl.getSelectedItem();
        if (item.isNull() || !item.isBlock()) {
            mc_utils::sendText<mc_utils::LogLevel::Error>(pl, "请手持一个方块!");
            return;
        }
        Config::dynamicAddSimilarBlock(typeName, item.mBlock->getTypeName());
        __sendEditSimilarBlock(pl, typeName);
    });
    f.appendDivider();
    for (auto const& similar : similarBlock) {
        ItemStack item{similar};
        f.appendButton(
            fmt::format("{}\n点击移除方块", item.isNull() && !item.isBlock() ? similar : item.getName()),
            [similar, typeName]([[maybe_unused]] Player& pl) {
                Config::dynamicRemoveSimilarBlock(typeName, similar);
                __sendEditSimilarBlock(pl, typeName);
            }
        );
    }
    f.sendTo(player);
}

void _sendEditBlockConfig(Player& player, std::string const& typeName) {
    auto const& block = Config::cfg.blocks[typeName];
    CustomForm  f{PLUGIN_NAME};

    f.appendInput("typeName", "命名空间", "string", typeName);
    f.appendInput("name", "名称", "string", block.name);
    f.appendInput("cost", "消耗经济", "int", std::to_string(block.cost));
    f.appendInput("limit", "最大连锁数量", "int", std::to_string(block.limit));

    f.appendDropdown("destroyMode", "破坏模式", DestroyModeType);
    f.appendDropdown("silkTouchMode", "精准采集模式", SilkTouchType);

    f.sendTo(player, [last = typeName](Player& pl, CustomFormResult const& res, FormCancelReason) {
        if (!res) return;
        try {
            std::string           typeName = std::get<std::string>(res->at("typeName"));
            std::string           name     = std::get<std::string>(res->at("name"));
            int                   cost     = std::stoi(std::get<std::string>(res->at("cost")));
            int                   limit    = std::stoi(std::get<std::string>(res->at("limit")));
            Config::DestroyMode   dmod     = DestroyModeMap.at(std::get<std::string>(res->at("destroyMode")));
            Config::SilkTouchMode smod     = SilkTouchMap.at(std::get<std::string>(res->at("silkTouchMode")));

            Config::dynamicUpdateBlockConfig(last, typeName, {name, cost, limit, dmod, smod});
            _sendBlockViewer(pl, typeName);
        } catch (...) {}
    });
}

void _addHandheldItemBlock(Player& player) {
    auto const& item = player.getSelectedItem();
    if (item.isNull()) {
        mc_utils::sendText<mc_utils::LogLevel::Error>(player, "请手持一个方块!");
    }

    if (!item.isBlock()) {
        mc_utils::sendText<mc_utils::LogLevel::Error>(player, "当前手持物品没有对应方块实例!");
        return;
    }
    auto block = item.mBlock;

    Config::dynamicAddBlockConfig(block->getTypeName(), {.name = item.getName()});
    _sendEditBlockConfig(player, block->getTypeName());
}

void _sendBlockViewer(Player& player, std::string const& typeName) {
    auto const& block = Config::cfg.blocks[typeName];

    SimpleForm{PLUGIN_NAME}
        .setContent(json_utils::struct2json(block).dump(2))
        .appendDivider()
        .appendButton(
            "编辑基础信息",
            "textures/ui/book_edit_hover",
            "path",
            [typeName](Player& pl) { _sendEditBlockConfig(pl, typeName); }
        )
        .appendButton(
            "编辑连锁工具",
            "textures/ui/book_edit_hover",
            "path",
            [typeName](Player& pl) { __sendEditBlockTools(pl, typeName); }
        )
        .appendButton(
            "编辑相似方块",
            "textures/ui/book_edit_hover",
            "path",
            [typeName](Player& pl) { __sendEditSimilarBlock(pl, typeName); }
        )
        .appendButton(
            "删除",
            "textures/ui/icon_trash",
            "path",
            [typeName](Player& pl) {
                Config::dynamicRemoveBlockConfig(typeName);
                PlayerConfig::removeBlock(pl.getUuid(), typeName);
                PlayerConfig::save();
                sendOpBlockManager(pl);
            }
        )
        .appendButton("返回", "textures/ui/icon_import", "path", [](Player& pl) { sendOpBlockManager(pl); })
        .sendTo(player);
}

void sendOpBlockManager(Player& player) {
    SimpleForm f{PLUGIN_NAME};
    f.setContent("FastMiner - 管理面板");

    f.appendButton("添加手持方块", "textures/ui/color_plus", "path", [](Player& pl) { _addHandheldItemBlock(pl); });
    f.appendDivider();

    for (auto& [k, v] : Config::cfg.blocks) {
        f.appendButton(v.name, [k](Player& pl) { _sendBlockViewer(pl, k); });
    }

    f.sendTo(player);
}


void sendPlayerConfigGUI(Player& player) {
    auto const& uuid = player.getUuid();

    CustomForm f{PLUGIN_NAME};
    f.appendToggle(PlayerConfig::KEY_ENABLE, "启用连锁采集", PlayerConfig::isEnabled(uuid, PlayerConfig::KEY_ENABLE));
    f.appendToggle(PlayerConfig::KEY_SNEAK, "仅潜行时启用", PlayerConfig::isEnabled(uuid, PlayerConfig::KEY_SNEAK));

    f.appendDivider();
    f.appendLabel(">>>  方块配置");
    for (auto const& [key, item] : Config::cfg.blocks) {
        f.appendToggle(key, item.name, PlayerConfig::isEnabled(uuid, key));
    }

    f.sendTo(player, [](Player& pl, CustomFormResult const& res, FormCancelReason) {
        if (!res) return;

        auto const& uuid = pl.getUuid();

        bool const enable = std::get<uint64_t>(res->at(PlayerConfig::KEY_ENABLE));
        bool const sneak  = std::get<uint64_t>(res->at(PlayerConfig::KEY_SNEAK));
        PlayerConfig::setEnabled(uuid, PlayerConfig::KEY_ENABLE, enable);
        PlayerConfig::setEnabled(uuid, PlayerConfig::KEY_SNEAK, sneak);

        for (auto& [k, v] : Config::cfg.blocks) {
            if (res->contains(k)) {
                PlayerConfig::setEnabled(uuid, k, std::get<uint64_t>(res->at(k)));
            }
        }
        PlayerConfig::save();
        mc_utils::sendText(pl, "设置已保存");
    });
}


} // namespace fm::gui