#pragma once
#include <string_view>

namespace fm {
namespace client {

struct ClientFastMinerCommand {
    ClientFastMinerCommand() = delete;

    static void setup(std::string_view name, std::string_view description);
};

} // namespace client
} // namespace fm
