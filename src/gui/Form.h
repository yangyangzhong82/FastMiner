#pragma once
#include "Config/Config.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/block/Block.h"
#include <mc/world/actor/Actor.h>
#include <mc/world/actor/player/Player.h>

namespace fm::gui {


void _sendEditBlockConfig(Player& player, std::string const& typeName);

void _addHandheldItemBlock(Player& player);

void _sendBlockViewer(Player& player, std::string const& typeName);

void sendOpBlockManager(Player& player);

void sendPlayerConfigGUI(Player& player);


} // namespace fm::gui