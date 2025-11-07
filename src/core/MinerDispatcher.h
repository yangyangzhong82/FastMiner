#pragma once
#include "Global.h"
#include "core/MinerTask.h"

#include "ll/api/coro/Executor.h"

#include "absl/container/flat_hash_map.h"
#include "mc/platform/UUID.h"

#include <coroutine>
#include <cstddef>


namespace fm {


/**
 * @brief 任务调度器
 * 调度模型:
 * 1. 任务创建后，挂起并交给调度器，由调度器分配任务许可额度
 * 2. 每 tick 一次，进行计算额度，并恢复挂起的任务执行
 * 3. 任务执行完毕后，将任务从调度器中移除
 *
 * @note 额度分配模型:
 *  int quota_per_task = max(1, floor(globalBlockLimitPerTick / active_tasks));
 *
 * @note 最大恢复任务受 maxResumeTasksPerTick 限制(避免瞬间卡死线程)
 */
class MinerDispatcher final {
public:
    FM_DISABLE_COPY_MOVE(MinerDispatcher);
    explicit MinerDispatcher();
    ~MinerDispatcher();

    inline bool isProcessing(HashedDimPos pos) const { return processingBlocks.contains(pos); }
    inline void insertProcessing(HashedDimPos pos) { processingBlocks.insert(pos); }
    inline void eraseProcessing(HashedDimPos pos) { processingBlocks.erase(pos); }

    bool canLaunchTask(Player& player) const;

    void launch(MinerTask::Ptr task);

    void enqueue(MinerTask* task, std::coroutine_handle<> h, ll::coro::ExecutorRef exec);

    void interruptPlayerTask(Player& player);

    void onTaskFinished(MinerTask* task);

    void tick();

private:
    absl::flat_hash_set<HashedDimPos>              processingBlocks; // 正在处理的方块
    absl::flat_hash_map<mce::UUID, MinerTask::Ptr> tasks_;           // 任务队列
    struct Pending {
        MinerTask*              task;
        std::coroutine_handle<> h;
        ll::coro::ExecutorRef   exec;
    };
    std::vector<Pending> pending_; // 等待执行的任务
};


} // namespace fm