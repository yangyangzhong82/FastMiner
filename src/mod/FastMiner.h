#pragma once
#include "core/MinerLauncher.h"
#include "ll/api/mod/NativeMod.h"
#include <memory>

namespace fm {

class MinerLauncher;

class FastMiner {
public:
    static FastMiner& getInstance();

    FastMiner() : mSelf(*ll::mod::NativeMod::current()) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    bool load();

    bool enable();

    bool disable();

    bool unload();

private:
    ll::mod::NativeMod&            mSelf;
    std::unique_ptr<MinerLauncher> mLauncher;
};

} // namespace fm
