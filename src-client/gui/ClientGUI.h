#pragma once
#include <string>

class Player;

namespace fm {
namespace client {

struct ClientGUI {
    ClientGUI() = delete;

    static void sendTo(Player& player);

    static void _sendBlockViewer(Player& player, std::string const& typeName);
    static void _sendEditSimilarBlock(Player& player, std::string const& typeName);
    static void _handleAddItemBlock(Player& player);
    static void _sendEditBlockConfig(Player& player, std::string const& typeName);
};

} // namespace client
} // namespace fm
