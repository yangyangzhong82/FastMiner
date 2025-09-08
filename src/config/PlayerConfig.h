#pragma once
#include "mc/platform/UUID.h"
#include <string>
#include <unordered_map>


namespace fm::PlayerConfig {


using BlockEnableState = std::unordered_map<std::string, bool>;
using Impl             = std::unordered_map<mce::UUID, BlockEnableState>;
inline Impl cfg;

void load();

void save();

bool isEnabled(mce::UUID const& uuid, std::string const& key);

void setEnabled(mce::UUID const& uuid, std::string const& key, bool enabled);

inline void enable(mce::UUID const& uuid, std::string const& key);

inline void disable(mce::UUID const& uuid, std::string const& key);

inline bool hasPlayer(mce::UUID const& uuid);
inline bool hasBlock(mce::UUID const& uuid, std::string const& key);
inline void removeBlock(mce::UUID const& uuid, std::string const& key);

void checkAndTryRemoveNotExistBlock();

constexpr inline auto KEY_ENABLE = "enable";
constexpr inline auto KEY_SNEAK  = "sneak";

} // namespace fm::PlayerConfig