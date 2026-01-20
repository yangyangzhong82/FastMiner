#pragma once
#include "IPlatformService.h"

#include <memory>

namespace fm {
namespace client {

class ClientPlatformService final : public IPlatformService {
    struct Impl;
    std::unique_ptr<Impl> impl;

public:
    ClientPlatformService();
    ~ClientPlatformService() override;

    bool init() override;
    bool destroy() override;
};

} // namespace client
} // namespace fm
