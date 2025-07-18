#include "config/Config.h"
#include "ll/api/chrono/GameChrono.h"
#include "ll/api/coro/CoroTask.h"
#include "ll/api/event/Event.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/ListenerBase.h"
#include "ll/api/event/player/PlayerDestroyBlockEvent.h"
#include "ll/api/service/Bedrock.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/item/ItemStackBase.h"
#include "mc/world/item/enchanting/Enchant.h"
#include "mc/world/item/enchanting/EnchantUtils.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/block/Block.h"
#include "mc/world/level/block/actor/BlockActor.h"
#include "mc/world/level/dimension/Dimension.h"
#include "mc/world/level/material/Material.h"
#include "mod/FastMiner.h"
#include <functional>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>


namespace fm::core {

void registerEvent();
void unRegisterEvent();

void miner(int const& taskID, BlockPos const stratPos);

size_t hash(BlockPos const& v3, int const& dim);
size_t hash(ll::event::player::PlayerDestroyBlockEvent const& ev);
int    randomInt();

} // namespace fm::core
