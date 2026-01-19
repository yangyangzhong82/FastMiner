#pragma once

namespace fm::client {

struct ClientConfig {
    static int constexpr SchemaVersion = 1;

    int version = SchemaVersion;
};

} // namespace fm::client