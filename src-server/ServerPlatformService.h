#pragma once
#include "IPlatformService.h"

#include <econbridge/IEconomy.h>

#include <memory>

namespace fm {
namespace server {

class ServerPlatformService final : public IPlatformService {
    struct Impl;
    std::unique_ptr<Impl> impl;

public:
    ServerPlatformService();
    ~ServerPlatformService() override;

    bool init() override;
    bool destroy() override;

    econbridge::IEconomy& getEconomy() const;
};

} // namespace server
} // namespace fm
