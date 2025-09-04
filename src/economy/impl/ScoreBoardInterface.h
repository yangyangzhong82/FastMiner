#pragma once
#include "IEconomyInterface.h"
#include "economy/impl/IEconomyInterface.h"

namespace fm ::internal {


class ScoreBoardInterface final : public IEconomyInterface {
public:
    explicit ScoreBoardInterface();

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
