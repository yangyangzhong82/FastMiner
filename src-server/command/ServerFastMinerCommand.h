#pragma once
#include <string_view>

namespace fm {
namespace server {

struct ServerFastMinerCommand {
    ServerFastMinerCommand() = delete;

    static void setup(std::string_view command, std::string_view description);
};

} // namespace server
} // namespace fm
