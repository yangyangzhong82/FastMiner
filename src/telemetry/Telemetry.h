#pragma once

#include "ll-bstats/Telemetry.h"

namespace fm {

class Telemetry final : public ll_bstats::Telemetry {
public:
    using ll_bstats::Telemetry::Telemetry;

    void initConstant() override;
};

} // namespace fm