#include "MinerDispatcher.h"
#include "config/Config.h"
#include "core/MinerTask.h"
#include "mc/world/actor/player/Player.h"

namespace fm {

MinerDispatcher::MinerDispatcher() { processingBlocks.reserve(128); }
MinerDispatcher::~MinerDispatcher() {}

bool MinerDispatcher::canLaunchTask(Player& player) const { return !tasks_.contains(player.getUuid()); }

void MinerDispatcher::launch(MinerTask::Ptr task) {
    if (!canLaunchTask(task->player)) {
        throw std::runtime_error("Player already has a task running");
    }
    tasks_.emplace(task->player.getUuid(), task);
    task->execute();
}

void MinerDispatcher::enqueue(MinerTask* task, std::coroutine_handle<> h, ll::coro::ExecutorRef exec) {
    pending_.push_back({task, h, exec});
}

void MinerDispatcher::interruptPlayerTask(Player& player) {
    auto iter = tasks_.find(player.getUuid());
    if (iter != tasks_.end()) {
        iter->second->interrupt();
    }
    tasks_.erase(iter);
}

void MinerDispatcher::onTaskFinished(MinerTask* task) { tasks_.erase(task->player.getUuid()); }

void MinerDispatcher::tick() {
    if (tasks_.empty()) return;

    auto const& cfg         = Config::cfg.dispatcher;
    int const&  globalLimit = cfg.globalBlockLimitPerTick;
    int const&  maxResume   = cfg.maxResumeTasksPerTick;

    int activeCount  = static_cast<int>(tasks_.size());
    int quotaPerTask = std::max(1, globalLimit / std::max(1, activeCount));

    int resumed = 0;
    for (size_t i = 0; i < pending_.size() && resumed < maxResume; ++i) {
        auto& p = pending_[i];
        if (p.task->canContinue()) {
            // 分配额度并恢复协程
            p.task->quota += quotaPerTask;
            p.h.resume();
        }
        resumed++;
    }
    // 清理已恢复的协程
    pending_.erase(pending_.begin(), pending_.begin() + resumed);
}

} // namespace fm