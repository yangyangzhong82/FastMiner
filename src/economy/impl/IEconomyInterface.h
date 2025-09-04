#pragma once
#include "economy/EconomyConfig.h"

class Player;
namespace mce {
class UUID;
};

namespace fm::internal {

using llong = long long;

class IEconomyInterface {
public:
    IEconomyInterface(const IEconomyInterface&)            = delete;
    IEconomyInterface& operator=(const IEconomyInterface&) = delete;

    virtual ~IEconomyInterface() = default;

    explicit IEconomyInterface();

public:
    virtual llong get(Player& player) const        = 0;
    virtual llong get(mce::UUID const& uuid) const = 0;

    virtual bool set(Player& player, llong amount) const        = 0;
    virtual bool set(mce::UUID const& uuid, llong amount) const = 0;

    virtual bool add(Player& player, llong amount) const        = 0;
    virtual bool add(mce::UUID const& uuid, llong amount) const = 0;

    virtual bool reduce(Player& player, llong amount) const        = 0;
    virtual bool reduce(mce::UUID const& uuid, llong amount) const = 0;

    virtual bool transfer(Player& from, Player& to, llong amount) const                   = 0;
    virtual bool transfer(mce::UUID const& from, mce::UUID const& to, llong amount) const = 0;

    virtual bool has(Player& player, llong amount) const;
    virtual bool has(mce::UUID const& uuid, llong amount) const;

public:
    virtual std::string getCostMessage(Player& player, llong amount) const;

    virtual void sendNotEnoughMoneyMessage(Player& player, llong amount) const;

    virtual EconomyConfig& getConfig() const;
};


} // namespace fm::internal