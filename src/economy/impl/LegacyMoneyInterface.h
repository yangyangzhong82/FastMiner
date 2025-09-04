#pragma once
#include "IEconomyInterface.h"
#include <optional>

namespace fm::internal {


class LegacyMoneyInterface final : public IEconomyInterface {
public:
    explicit LegacyMoneyInterface();

    static bool isLegacyMoneyLoaded();

    std::optional<std::string> getXuidFromPlayerInfo(mce::UUID const& uuid) const; // 从 PlayerInfo 获取 Xuid

public: // override
    llong get(Player& player) const override;
    llong get(mce::UUID const& uuid) const override;

    bool set(Player& player, llong amount) const override;
    bool set(mce::UUID const& uuid, llong amount) const override;

    bool add(Player& player, llong amount) const override;
    bool add(mce::UUID const& uuid, llong amount) const override;

    bool reduce(Player& player, llong amount) const override;
    bool reduce(mce::UUID const& uuid, llong amount) const override;

    bool transfer(Player& from, Player& to, llong amount) const override;
    bool transfer(mce::UUID const& from, mce::UUID const& to, llong amount) const override;
};


} // namespace fm::internal