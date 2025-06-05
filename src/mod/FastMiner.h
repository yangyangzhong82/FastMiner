#pragma once

#include "ll/api/mod/NativeMod.h"

namespace fm {

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
    ll::mod::NativeMod& mSelf;
};

} // namespace fm
