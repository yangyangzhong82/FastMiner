#pragma once
#include "mc/nbt/ByteTag.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/level/BlockPos.h"

#include <cstddef>
#include <string_view>


namespace fm {


using HashedDimPos = size_t; // Hashed dimension position

namespace miner_util {

inline HashedDimPos hashDimensionPosition(BlockPos const& pos, int const& dim) {
    constexpr size_t prime1 = 73856093;
    constexpr size_t prime2 = 19349663;
    constexpr size_t prime3 = 83492791;
    return (static_cast<size_t>(pos.x) * prime1) ^ (static_cast<size_t>(pos.y) * prime2)
         ^ (static_cast<size_t>(pos.z) * prime3) ^ (static_cast<size_t>(dim) << 16);
}

inline bool hasUnbreakable(ItemStack const& item) {
    static constexpr std::string_view unbreakable = "Unbreakable";

    auto& nbt = item.mUserData;
    if (!nbt->contains("tag")) {
        return false;
    }
    auto& tag = (*nbt)["tag"];
    if (tag.contains(unbreakable)) {
        return tag[unbreakable].get<ByteTag>();
    }
    return false;
}

} // namespace MinerUtil


} // namespace fm