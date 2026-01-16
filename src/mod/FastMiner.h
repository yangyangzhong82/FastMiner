#pragma once
#include "ll/api/mod/NativeMod.h"
#include <memory>

#ifdef LL_PLAT_S
#include "econbridge/IEconomy.h"
#endif


namespace fm {

class MinerLauncher;

class FastMiner {
public:
    static FastMiner& getInstance();

    FastMiner();

    [[nodiscard]] ll::mod::NativeMod& getSelf();

    bool load();

    bool enable();

    bool disable();

    bool unload();

#ifdef LL_PLAT_S
    [[nodiscard]] econbridge::IEconomy& getEconomy() const;
#endif

private:
    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

} // namespace fm
