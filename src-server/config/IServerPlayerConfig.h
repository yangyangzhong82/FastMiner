#pragma once
#include <mc/platform/UUID.h>

#include <string_view>

namespace fm::server {

class IServerPlayerConfig {
public:
    virtual ~IServerPlayerConfig() = default;

    inline static constexpr std::string_view PlayerConfigFileName = "PlayerSetting.json";
    inline static constexpr std::string_view KEY_ENABLE           = "enable";
    inline static constexpr std::string_view KEY_SNEAK            = "sneak";

    using BlockState       = std::unordered_map<std::string, bool>;
    using PlayerBlockState = std::unordered_map<mce::UUID, BlockState>;

    virtual bool isEnabled(mce::UUID const& uuid, std::string const& key) = 0;

    virtual void setEnabled(mce::UUID const& uuid, std::string const& key, bool enabled) = 0;

    virtual void enable(mce::UUID const& uuid, std::string const& key) = 0;

    virtual void disable(mce::UUID const& uuid, std::string const& key) = 0;

    virtual bool hasPlayer(mce::UUID const& uuid)                                = 0;
    virtual bool hasBlock(mce::UUID const& uuid, std::string const& key)    = 0;
    virtual void removeBlock(mce::UUID const& uuid, std::string const& key) = 0;
    virtual void ensurePlayerBlockConfig()                                       = 0;
};

} // namespace fm::server