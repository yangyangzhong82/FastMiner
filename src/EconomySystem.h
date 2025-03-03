#pragma once
#include "mc/world/actor/player/Player.h"


namespace fm {

enum class EconomyKit : int { LegacyMoney, ScoreBoard };

struct EconomySystemConfig {
    bool        enabled{true};                // 是否启用
    EconomyKit  kit{EconomyKit::LegacyMoney}; // 经济系统
    std::string economyName{"money"};         // 经济名称
    std::string scoreboardObjName;            // 计分板对象名称
};

class EconomySystem {
public:
    EconomySystem()                                = default;
    EconomySystem(const EconomySystem&)            = delete;
    EconomySystem& operator=(const EconomySystem&) = delete;
    EconomySystem(EconomySystem&&)                 = delete;
    EconomySystem& operator=(EconomySystem&&)      = delete;

    [[nodiscard]] static EconomySystem& getInstance();

    [[nodiscard]] long long get(Player& player);

    bool set(Player& player, long long money);

    bool add(Player& player, long long money);

    bool reduce(Player& player, long long money);

    [[nodiscard]] std::string getSpendTip(Player& player, long long money);

    void sendLackMoneyTip(Player& player, long long money); // 发送经济不足提示
};


} // namespace fm