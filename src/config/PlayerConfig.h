#pragma once
#include "mc/platform/UUID.h"
#include <string>
#include <unordered_map>


namespace fm::PlayerConfig {


using Impl = std::unordered_map<mce::UUID, std::unordered_map<std::string, bool>>;
inline Impl cfg;

void load();

void save();

bool isEnabled(mce::UUID const& uuid, std::string const& key);

void setEnabled(mce::UUID const& uuid, std::string const& key, bool enabled);

void enable(mce::UUID const& uuid, std::string const& key);

void disable(mce::UUID const& uuid, std::string const& key);

constexpr inline auto KEY_ENABLE = "enable";
constexpr inline auto KEY_SNEAK  = "sneak";

} // namespace fm::PlayerConfig