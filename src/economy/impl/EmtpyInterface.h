#pragma once
#include "IEconomyInterface.h"


namespace fm::internal {


/**
 * @brief 空的经济系统, 什么也不做，全返回成功，用于禁用经济系统
 */
class EmtpyInterface final : public IEconomyInterface {
public:
    explicit EmtpyInterface();

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