#include "Telemetry.h"

#include "ll-bstats/bstats/Bukkit.h"

#include "magic_enum.hpp"

#include <string_view>

namespace fm {

enum class Environment {
    Unknown,
    DedicatedServer, // BDS
    BuiltinServer,   // C/S
    Client           // C/S
};

void Telemetry::initConstant() {
    ll_bstats::Telemetry::initConstant();

    Environment currentEnvironment = Environment::Unknown;
#ifdef LL_PLAT_S
    currentEnvironment = Environment::DedicatedServer;
#elif LL_PLAT_C
    currentEnvironment = Environment::BuiltinServer;
    // todo: add client logic
#endif

    payload.addCustomChart(
        bstats::bukkit::SimplePie{"execution_side", magic_enum::enum_name(currentEnvironment).data()}
    );
}

} // namespace fm