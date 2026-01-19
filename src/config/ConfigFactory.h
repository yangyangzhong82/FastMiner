#pragma once
#include "Type.h"

#include <memory>

#include "trait/ConfigFactoryTrait.h"

namespace fm {


struct ConfigFactory {
    using Factory = internal::ImplType<tag::ConfigFactoryTag>::type;

    static ConfigBase& getInstance() {
        static std::unique_ptr<ConfigBase> instance = Factory{}();
        return *instance;
    }
};


} // namespace fm