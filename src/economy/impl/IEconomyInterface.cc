#include "IEconomyInterface.h"
#include "economy/EconomySystem.h"
#include "ll/api/i18n/I18n.h"
#include "mc/world/actor/player/Player.h"


namespace fm ::internal {

using ll::operator""_trl;

EconomyConfig& IEconomyInterface::getConfig() const { return EconomySystem::getInstance().getConfig(); }


IEconomyInterface::IEconomyInterface() = default;

bool IEconomyInterface::has(Player& player, llong amount) const { return get(player) >= amount; }
bool IEconomyInterface::has(mce::UUID const& uuid, llong amount) const { return get(uuid) >= amount; }

std::string IEconomyInterface::getCostMessage(Player& player, long long amount) const {
    auto& config = getConfig();
    if (!config.enabled) {
        return "\n[Tip] 经济系统未启用，本次操作不消耗 {}"_trl(player.getLocaleCode(), config.economyName);
    }

    llong currentMoney = get(player);
    bool  isEnough     = currentMoney >= amount;

    return "\n[Tip] 本次操作需要: {0} {1} | 当前余额: {2} | 剩余余额: {3} | {4}"_trl(
        player.getLocaleCode(),
        amount,
        config.economyName,
        currentMoney,
        currentMoney - amount,
        isEnough ? "余额充足"_trl(player.getLocaleCode()) : "余额不足"_trl(player.getLocaleCode())
    );
}

void IEconomyInterface::sendNotEnoughMoneyMessage(Player& player, long long amount) const {
    auto& config = getConfig();

    player.sendMessage(
        "§c[IEconomyInterface] 操作失败，需要 {0} {1}，当前余额 {2}"_trl(
            player.getLocaleCode(),
            amount,
            config.economyName,
            get(player)
        )
    );
}


} // namespace fm::internal