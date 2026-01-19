#pragma once
#include "Type.h"

#include "trait/CommandTrait.h"

namespace fm {

struct FastMinerCommand {
    FastMinerCommand() = delete;

    inline static constexpr std::string_view CommandName        = "fm";
    inline static constexpr std::string_view CommandDescription = "FastMiner - 连锁采集";

    using Impl = typename internal::ImplType<tag::CommandTag>::type;

    static_assert(requires {
        { Impl::setup(CommandName, CommandDescription) } -> std::same_as<void>;
    });

    inline static void setup() { Impl::setup(CommandName, CommandDescription); }
};

} // namespace fm