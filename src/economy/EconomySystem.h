#pragma once
#include "EconomyConfig.h"
#include "economy/impl/IEconomyInterface.h"
#include <memory>
#include <mutex>
#include <string>


class Player;
namespace mce {
class UUID;
}

namespace fm {


class EconomySystem final {
    std::shared_ptr<internal::IEconomyInterface> mEconomySystem;
    mutable std::mutex                           mInstanceMutex;

    explicit EconomySystem();


public:
    EconomySystem(const EconomySystem&)            = delete;
    EconomySystem& operator=(const EconomySystem&) = delete;
    EconomySystem(EconomySystem&&)                 = delete;
    EconomySystem& operator=(EconomySystem&&)      = delete;

    static EconomySystem& getInstance();

    void initEconomySystem();   // 初始化经济系统
    void reloadEconomySystem(); // 重载经济系统（当 kit 改变时）

    EconomyConfig& getConfig() const;

    std::shared_ptr<internal::IEconomyInterface> getEconomyInterface() const;

    std::shared_ptr<internal::IEconomyInterface> operator->() const;

private:
    std::shared_ptr<internal::IEconomyInterface> createEconomySystem() const;
};


} // namespace fm
